# Run ldd on project executable and copy libraries to deploy folder
import os
import shutil
import re

# Find project version in CMakeLists.txt
def find_project_version():
    with open('CMakeLists.txt', 'r') as file:
        content = file.read()
        version_match = re.search(r'project\(.*VERSION\s+(\d+\.\d+\.\d+)\)', content)
        if version_match:
            return version_match.group(1)
        else:
            return None

project_version = find_project_version()

# Find project executable in current directory (Prefer file with "Release" postfix, otherwise "Debug")
executable_path = ""

for file in os.listdir(os.getcwd()):
    if file.endswith(project_version + "_Release.exe"):
        executable_path = os.path.join(os.getcwd(), file)
        break
    elif file.endswith(project_version + "_Debug.exe") and not executable_path.endswith(project_version + "_Release.exe"):
        executable_path = os.path.join(os.getcwd(), file)

# If no executable was found, search for the executable in the Debug or Release folders
if executable_path == "":
    if os.path.exists(os.getcwd() + "\\Release"):
        for file in os.listdir(os.getcwd() + "\\Release"):
            if file.endswith(project_version + "_Release.exe"):
                executable_path = os.path.join(os.getcwd() + "\\Release" , file)
                break

    if os.path.exists(os.getcwd() + "\\Debug"):
        for file in os.listdir(os.getcwd() + "\\Debug"):
            if file.endswith(project_version + "_Debug.exe") and not executable_path.endswith(project_version + "_Release.exe"):
                executable_path = os.path.join(os.getcwd() + "\\Debug", file)

print ("Executable path: " + executable_path)

if executable_path == "":
    print("No executable found!")
    exit(1)

# Path to the deploy folder
deploy_folder = "./Deploy/" + project_version + "/"

# Delete folder if exists
if os.path.exists(deploy_folder):
    print("Deploy folder exists: " + deploy_folder + ", deleting...")
    shutil.rmtree(deploy_folder)

# Create deploy folder
os.makedirs(deploy_folder)

# Run ldd on the executable
ldd_output = os.popen("ldd " + executable_path).read()

# Parse the ldd output to get the library paths
library_paths = []
for line in ldd_output.splitlines():
    if "=>" in line:
        # Add path to library_path if it is not equal to "not found"
        if "not found" not in line:
            matches = re.findall("^\s*([\w\.\d+-]+) => ([/\w\.\d+ -]+)(?: \(.*\))?", line)
            library_path = matches[0][1].replace("/c/", "C:/").strip()

            # Append if the library not from Windows folder
            if not library_path.startswith("C:/Windows"):
                library_paths.append(library_path)
                print("Library found: " + library_path)
        else:
            print("Library not found: " + line.split("=>")[0].strip())

# Copy the libraries to the deploy folder
for library_path in library_paths:
    print("Copying library: " + library_path + " to deploy folder: " + deploy_folder)
    shutil.copy(library_path, deploy_folder)

# Copy the executable to the deploy folder
shutil.copy(executable_path, deploy_folder)

# Copy assets folder if exists
if os.path.exists("./Assets/"):
    print("Copying assets folder to deploy folder: " + deploy_folder)
    shutil.copytree("./Assets/", deploy_folder + "/Assets/")

# Copy shaders folder if exists
if os.path.exists("./Shaders/"):
    print("Copying shaders folder to deploy folder: " + deploy_folder)
    shutil.copytree("./Shaders/", deploy_folder + "/Shaders/")

# Open deploy folder in explorer
os.startfile(os.path.abspath(deploy_folder))