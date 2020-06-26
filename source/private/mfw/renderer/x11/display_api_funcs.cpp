#include <private/mfw/renderer/x11/display_api_funcs.hpp>
#include <private/mfw/renderer/renderer.hpp>

#include <dirent.h>
#include <cstring>

namespace mfw::renderer
{
	MFW_DECLARE_LOG_CONTEXT(log_display_api_x11, u8"renderer/display_api/x11"_p)

	namespace __display_api_funcs_internal
	{
		static void read_file_int(ucstring_view path, size_t &i)
		{
			FILE *tmp_file{fopen(c_str(path), "r")};
			if(!tmp_file) {
				return;
			}

			ucstring str{};
			while(true) {
				char c{'\0'};
				if(!fread(&c, sizeof(char), 1, tmp_file) ||
					c == '\0' ||
					c == '\n') {
					break;
				}
				str += c;
			}

			fclose(tmp_file);

			str.erase(str.begin(), str.cbegin()+2);

			to_int(str, i, 16);
		}

		static void skip_whitespace(ucstring &str, size_t &idx, bool devices) {
			while(true) {
				bool valid{false};
				if(str[idx] == u8'\0' ||
					str[idx] == u8'\n' ||
					str[idx] == u8' ') {
					valid = true;
				}
				if(!devices) {
					if(str[idx] == u8'\t') {
						valid = true;
					}
				}
				if(valid) {
					idx++;
				} else {
					break;
				}
			}
		}

		static void skip_comments(ucstring &str, size_t &idx, bool devices)
		{
			skip_whitespace(str, idx, devices);
			while(str[idx] == u8'#') {
				idx = str.find(u8'\n', idx);
				idx++;
				skip_whitespace(str, idx, devices);
			}
		}

		static void skip_devices(ucstring &str, size_t &idx)
		{
			skip_comments(str, idx, true);
			while(str[idx] == u8'\t') {
				idx = str.find(u8'\n', idx);
				idx++;
				skip_comments(str, idx, true);
			}
		}

		static void skip_subdevices(ucstring &str, size_t &idx)
		{
			skip_comments(str, idx, true);
			while(str[idx] == u8'\t' &&
					str[idx+1] == u8'\t') {
				idx = str.find(u8'\n', idx);
				idx++;
				skip_comments(str, idx, true);
			}
		}


		static struct pci_cache_t {
			ucstring str{};
			using vendor_map_t = unordered_map<uint32_t, pair<ucstring, size_t>>;
			vendor_map_t vendor_map{};
			void clear() {
				str.clear();
				vendor_map.clear();
			}
			void load() {
				if(str.empty()) {
					static FILE *tmp_file{fopen("/usr/share/hwdata/pci.ids", "r")};
					if(!tmp_file) {
						return;
					}
					while(true) {
						char c{'\0'};
						if(!fread(&c, sizeof(char), 1, tmp_file) ||
							c == '\0') {
							break;
						}
						str += c;
					}
					fclose(tmp_file);
				}
			}
		} pci_cache{};

		static void get_device_name(uint32_t vendor, uint32_t device, ucstring &name)
		{
			pci_cache.load();

			size_t idx{0};
			pci_cache_t::vendor_map_t::iterator it{pci_cache.vendor_map.find(vendor)};
			if(it != pci_cache.vendor_map.end()) {
				idx = it->second.second;
				name += it->second.first;
			} else {
				while(true) {
					skip_comments(pci_cache.str, idx, false);

					size_t end{pci_cache.str.find(u8' ', idx)};
					ucstring vend_id_str{pci_cache.str.substr(idx, (end-idx))};

					uint32_t vend_id{0};
					to_int(vend_id_str, vend_id, 16);

					idx = pci_cache.str.find(u8'\n', end);
					idx++;

					if(vend_id != vendor) {
						skip_devices(pci_cache.str, idx);
						continue;
					}

					idx = end;

					skip_whitespace(pci_cache.str, idx, false);

					end = pci_cache.str.find(u8'\n', idx);

					name += pci_cache.str.substr(idx, (end-idx));
					name += u8' ';

					idx = end;
					idx++;

					pci_cache.vendor_map.emplace(pci_cache_t::vendor_map_t::value_type{idx, {name, idx}});
					break;
				}
			}

			while(true) {
				skip_comments(pci_cache.str, idx, true);

				if(pci_cache.str[idx] != u8'\t') {
					break;
				}

				idx++;
				size_t end{pci_cache.str.find(u8' ', idx)};
				ucstring dev_id_str{pci_cache.str.substr(idx, (end-idx))};

				uint32_t dev_id{0};
				to_int(dev_id_str, dev_id, 16);

				idx = pci_cache.str.find(u8'\n', end);
				idx++;

				if(dev_id != device) {
					skip_subdevices(pci_cache.str, idx);
					continue;
				}

				idx = end;

				skip_whitespace(pci_cache.str, idx, false);

				end = pci_cache.str.find(u8'\n', idx);

				name += pci_cache.str.substr(idx, (end-idx));
				break;
			}
		}
	}

	bool display_api_funcs_x11::collect_gpus(ptr_vector<graphics_card> &gpus)
	{
		constexpr ucstring_view dir{u8"/sys/bus/pci/devices"_sv};
		DIR *devdir{opendir(c_str(dir))};
		if(devdir) {
			dirent *entry{nullptr};
			while(true) {
				entry = readdir(devdir);
				if(!entry) {
					break;
				}

				if((entry->d_name[0] == '.') ||
					(entry->d_name[0] == '.' && entry->d_name[1] == '.')) {
					continue;
				}

				ucstring tmp{dir};
				tmp += u8'/';
				tmp.append(uc_str(entry->d_name), static_cast<size_t>(strlen(entry->d_name)));
				tmp.append(u8"/class"_sv);

				size_t class_id{0};
				__display_api_funcs_internal::read_file_int(tmp, class_id);

				#define __MFW_DEVICE_CLASS_VGA 0x030000
				#define __MFW_DEVICE_CLASS_XGA 0x030100
				#define __MFW_DEVICE_CLASS_3D 0x030200
				#define __MFW_DEVICE_CLASS_OTHER 0x038000

				if(class_id != __MFW_DEVICE_CLASS_VGA &&
					class_id != __MFW_DEVICE_CLASS_XGA &&
					class_id != __MFW_DEVICE_CLASS_3D &&
					class_id != __MFW_DEVICE_CLASS_OTHER) {
					continue;
				}

				tmp.erase(tmp.cend()-6, tmp.cend());
				tmp.append(u8"/vendor"_sv);

				size_t vendor_id{0};
				__display_api_funcs_internal::read_file_int(tmp, vendor_id);

				tmp.erase(tmp.cend()-7, tmp.cend());
				tmp.append(u8"/device"_sv);

				size_t device_id{0};
				__display_api_funcs_internal::read_file_int(tmp, device_id);

				graphics_card &gpu{gpus.emplace_back()};
				gpu.vendor = vendor_id;
				gpu.device = device_id;
				__display_api_funcs_internal::get_device_name(vendor_id, device_id, gpu.name);

				log_display_api_x11().info(u8"found {}"_sv, gpu.name);

				if(!graphics_card::maingpu) {
					bool valid{false};
					if(!mfw::renderer::renderer::instance().wants_integrated()) {
						valid = (class_id == __MFW_DEVICE_CLASS_VGA);
					} else {
						valid = (class_id == __MFW_DEVICE_CLASS_OTHER);
					}
					if(valid) {
						graphics_card::maingpu = &gpu;
					}
				}
			}
			if(!graphics_card::maingpu) {
				graphics_card::maingpu = &gpus[0];
			}
			log_display_api_x11().info(u8"selected {} as main gpu"_sv, graphics_card::maingpu->name);
			__display_api_funcs_internal::pci_cache.clear();
			closedir(devdir);
			return !gpus.empty();
		} else {
			return false;
		}
	}
}