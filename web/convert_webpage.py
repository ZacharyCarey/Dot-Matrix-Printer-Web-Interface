#apt install python3.12-venv
# python3 -m venv my-venv
# my-venv/bin/pip install htmlmin
# my-venv/bin/pip install jsmin
# my-venv/bin/python convert_webpage.py

import re
import htmlmin
from jsmin import jsmin

script_regex = r'<script\s+src\s*=\s*"?([^"]*)"?\s*>\s*<\/script>'

with open("index.html", 'rb') as f:
    html = f.read()



minified = htmlmin.minify(html.decode("utf-8"), remove_empty_space=True)
while True:
    match = re.search(script_regex, minified)
    if match is None:
        break
    scriptPath = match.group(1)
    with open(scriptPath) as js_file:
        script = jsmin(js_file.read())
    minified = minified[:match.span()[0]] + r'<script>' + script + r'</script>' + minified[match.span()[1]:]

with open("../Dot-Matrix-Printer-Web-Interface/src/index.h", "w") as file:
    file.write("#ifndef WEBPAGE_H\n")
    file.write("#define WEBPAGE_H\n")
    file.write("\n")
    file.write("const char* webpage = R\"=====(")
    file.write(minified)
    file.write(")=====\";\n")
    file.write("\n")
    file.write("#endif")