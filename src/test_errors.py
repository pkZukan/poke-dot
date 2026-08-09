import sys
import json
import urllib.request

url = "https://raw.githubusercontent.com/godotengine/godot/master/servers/rendering/rendering_server.cpp"
req = urllib.request.Request(url)
with urllib.request.urlopen(req) as response:
    lines = response.read().decode('utf-8').split('\n')

for i, line in enumerate(lines):
    if "array.size() != p_vertex_array_len * 4" in line:
        print(f"Line {i+1}: {line.strip()}")
        # print 5 lines before
        for j in range(max(0, i-5), i):
            print(f"  {lines[j].strip()}")

