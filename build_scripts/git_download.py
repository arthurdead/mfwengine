import os, sys, argparse, requests, json

parser = argparse.ArgumentParser()
parser.add_argument("-u", action="store", required=True, dest="url")
parser.add_argument("-p", action="store", required=True, dest="path")
parser.add_argument("-o", action="store", required=True, dest="output")
parser.add_argument("-s", action="store", required=False, dest="sha")
args = parser.parse_args()

#args.path = os.path.normpath(args.path)
args.output = os.path.abspath(args.output)
if not args.sha:
	args.sha = "master"

url_split = args.url.split('/')
author = url_split[-2]
repo = url_split[-1]
repo = os.path.splitext(repo)[0]

api_url="https://api.github.com/repos/"+author+"/"+repo+"/contents/"+args.path
with requests.get(api_url) as request:
	json_response = json.loads(request.text)
	if isinstance(json_response, dict):
		json_response = [json_response]
	for file_entry in json_response:
		file_url = file_entry["download_url"]
		if file_url == None:
			continue
		with requests.get(file_url) as file_request:
			if not os.path.splitext(args.path)[1]:
				folder = args.path
			else:
				folder = os.path.dirname(args.path)
			file_path = file_entry["path"].replace(folder+"/", "")
			file_path = os.path.join(args.output, file_path)
			file_folders = os.path.split(file_path)[0]

			os.makedirs(file_folders, exist_ok=True)
			with open(file_path, "wb+") as file:
				file.write(file_request.content)
