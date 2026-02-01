#!/usr/bin/env python3
# do not remove this comment, it creates entry point
import sys
import os
import json
import requests
import time

# initial available packages (for testing)
start_time = time.time()

def current_time():
    return time.time()

timeout = 5
# fix this with a lambda script to check timeout scripts.
while (start_time + timeout) > current_time():
    r = requests.get("https://pentagonx.github.io/beryllium-packages/packages.txt")
    if (start_time + timeout) < current_time():
        print("Operation Failed, offline mode detected!")
        break

print(r)
response = r.text
packages_unfiltered = response.splitlines()
packages = []
package_versions = {}
for line in packages_unfiltered:
    line = line.strip()
    split_pos = line.find("@")
    if split_pos == -1:
        print("Warning: invalid packages, can be ignored.")
    else:
        pkg_name = line[:split_pos]
        if pkg_name not in packages:
            packages.append(pkg_name)
        version = line[split_pos + 1:]
        if pkg_name not in package_versions:
            package_versions[pkg_name] = []
        package_versions[pkg_name].append(version)


# bervenv directory
BERVENV_DIR = "__bervenv__"
SYSPACKS = os.path.join(BERVENV_DIR, "syspacks")
PACKAGES_TXT = os.path.join(BERVENV_DIR, "syspacks/packages.txt")
PACKAGES_JSON = os.path.join(BERVENV_DIR, "syspacks/packages.json")

# utility to read installed packages


def read_installed_packages():
    if not os.path.isdir(BERVENV_DIR):
        return None  # venv not created
    installed = []
    if os.path.isfile(PACKAGES_TXT):
        with open(PACKAGES_TXT, "r") as f:
            for line in f:
                line = line.strip()
                if line:
                    # store as package@version if present
                    installed.append(line)
    return installed

# utility to write installed packages


def write_installed_packages(installed):
    if not os.path.isdir(BERVENV_DIR):
        print("Bervenv not created.")
        return

    if not os.path.isdir(SYSPACKS):
        print("Bervenv is broken, and malformed. Please recreate __bervenv__.")
        print("Syspacks not found.")
        return
    # update packages.txt
    with open(PACKAGES_TXT, "w") as f:
        for pkg in installed:
            f.write(f"{pkg}\n")
    # update packages.json
    with open(PACKAGES_JSON, "w") as f:
        json.dump({"packages": installed}, f, indent=4)

# parse package@version


def parse_package_arg(arg):
    if "@" in arg:
        name, version = arg.split("@", 1)
    else:
        name, version = arg, None
    return name, version

def install_package(name, version):
    r = requests.get("https://pentagonx.github.io/beryllium-packages/" + name + "/modules.txt")
    response = r.text
    
    modules = response.splitlines()
    for element in modules:
        if len(element.strip()) > 51:
            print(f"Warning: Invalid module name found. Can be ignored. [ERR: LONG_MODULE_NAME]")
            if name == element.strip():
                print("Failure: Package is invalid due to module name being too long. Aborting installation. Please raise this to the package managers.")
                return
            else:
                pass
    package_dir = os.path.join(SYSPACKS, name)
    os.makedirs(package_dir, exist_ok=True)
    for module in modules:
        module_dir = os.path.join(package_dir, module)
        os.makedirs(module_dir, exist_ok=True)
    
    print("Downloaded package modules.")

def main():
    argc = len(sys.argv)
    argv = sys.argv
    if argc <= 1:
        print("Usage: minoil [command] [options]")
        print("Run 'minoil help' for more information.")
        return

    command = argv[1].lower()
    installed = read_installed_packages()
    if not command == "help":
        if installed is None:
            print("Bervenv not created. Please create __bervenv__ first.")
            return

    # detect --force flag
    force = "--force" in argv
    # remove it from argv for simplicity
    argv = [arg for arg in argv if arg != "--force"]

    if command == "install":
        if argc < 3:
            print("Usage: minoil install [package] [--force]")
            return
        package_arg = argv[2]
        name, version = parse_package_arg(package_arg)

        if name not in packages:
            print(f"Error: Package '{name}' not found from packages: {packages}.")
            return

        # default to latest version if not specified
        if not version:
            version = package_versions.get(name, [])[-1]

        full_package = f"{name}@{version}"

        # check if same package installed
        installed_names = [p.split("@")[0] for p in installed]
        if name in installed_names:
            idx = installed_names.index(name)
            old_version = installed[idx].split(
                "@")[1] if "@" in installed[idx] else None
            if old_version == version:
                print(f"Package '{full_package}' is already installed.")
            else:
                action = "upgraded" if version > old_version else "downgraded"
                if force:
                    print(
                        f"Force {action} '{name}' from {old_version} to {version}...")
                    installed[idx] = full_package
                    write_installed_packages(installed)
                    print(f"Package '{full_package}' {action} successfully!")
                else:
                    print(
                        f"Package '{name}' already installed with version {old_version}.")
                    print(f"Use --force to {action} to {version}.")
        else:
            print(f"Installing package: {full_package}...")
            installed.append(full_package)
            install_package(name, version)
            write_installed_packages(installed)
            print(f"Package '{full_package}' installed successfully!")

    elif command == "uninstall":
        if argc < 3:
            print("Usage: minoil uninstall [package] [--force]")
            return
        package_arg = argv[2]
        name, version = parse_package_arg(package_arg)
        # remove all versions if version not specified
        to_remove = [p for p in installed if p.split("@")[0] == name]
        if not to_remove:
            print(f"Package '{name}' is not installed.")
            return
        if version:
            specific = f"{name}@{version}"
            if specific in installed:
                print(f"Uninstalling package: {specific}...")
                installed.remove(specific)
                write_installed_packages(installed)
                print(f"Package '{specific}' uninstalled successfully!")
            else:
                print(f"Package '{specific}' is not installed.")
        else:
            print(f"Uninstalling package(s) for: {name}...")
            for pkg in to_remove:
                installed.remove(pkg)
            write_installed_packages(installed)
            print(f"Package(s) '{name}' uninstalled successfully!")

    elif command == "list":
        if not installed:
            print("No packages installed.")
        else:
            print("Installed packages:")
            for pkg in installed:
                print(f"- {pkg}")

    elif command == "help":
        print("""
-------- Mineral Oil --------
- Beryllium Package Manager -
-----------------------------
Usage: minoil [command] [options]
Usage: mineraloil [command] [options]
Commands:
    install [package[@version]]   Install a package (latest version if not specified)
    uninstall [package[@version]] Uninstall a package (all versions if not specified)
    list                          List installed packages
    help                          Show this help message
""")
    else:
        print(f"Error: Command '{command}' not found or missing arguments.")


if __name__ == "__main__":
    main()
