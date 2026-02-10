package main

import (
	"bufio"
	"errors"
	"fmt"
	"os"
	fp "path/filepath"
)

const HelpText = `
-------- Mineral Oil --------
- Beryllium Package Manager -
-----------------------------
Usage: minoil [command] [options]
Usage: mineraloil [command] [options]

Commands:
install [package[@version]]   Install a package (latest version if not specified)
    --force                   Installs package with force, may corrupt packages, (use caustiosly)
uninstall [package[@version]] Uninstall a package (all versions if not specified)
    --force                   Uninstalls a package with force, may corrupt packages, (use caustiosly)
list                          List installed packages
help                          Show this help message
`

const BERVENV_DIR = "__bervenv__"

var SYSPACKS = fp.Join(BERVENV_DIR, "syspacks")
var PACKAGES_TXT = fp.Join(SYSPACKS, "packages.txt")
var PACKAGES_JSON = fp.Join(SYSPACKS, "packages.json")

func IsDir(path string) (bool, error) {
	fileInfo, err := os.Stat(path)
	if err != nil {
		return false, err
	}

	return fileInfo.IsDir(), nil
}

func ReadInstalledPackages() ([]string, error) {
	isDir, err := IsDir(BERVENV_DIR)
	if err != nil {
		return []string{}, err
	}
	if !isDir {
		return []string{}, errors.New("__bervenv__ is not a directory")
	}
	installed := []string{}

	if _, err := os.Stat(PACKAGES_TXT); err != nil {
		file, err := os.Open(PACKAGES_TXT)
		if err != nil {
			return installed, err
		}
		defer file.Close()
		scanner := bufio.NewScanner(file)
		for scanner.Scan() {
			line := scanner.Text()
			if line != "" {
				installed = append(installed, line)
			}
		}

		if err := scanner.Err(); err != nil {
			return installed, nil
		}
	}

	return installed, nil
}

func WriteInstalledPackages(installed []string) error {
	{
		isDir, err := IsDir(BERVENV_DIR)
		if err != nil {
			return err
		}
		if !isDir {
			fmt.Println("Bervenv not created")
		}
	}

	{
		isDir, err := IsDir(SYSPACKS)
		if err != nil {
			return err
		}
		if !isDir {
			fmt.Println("Bervenv is broken, and malformed")
		}
	}
	return nil
}

func main() {
	fmt.Println("Hello, world!")
}
