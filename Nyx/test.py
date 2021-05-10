from os import walk

f = []
path = "C:/dev/Nyx/Nyx/src/Nyx/graphics/ImGui"
for (path, dirnames, filenames) in walk(path):
    f.extend(filenames)
    break

print(f)