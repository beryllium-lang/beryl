package helpers

import (
	"fmt"
	"os"
	fp "path/filepath"
)

func FindBervenv() (string, error) {
	currPath, err := os.Getwd()
	if err != nil {
		return "", err
	}

	for {
		tryBervenv := fp.Join(currPath, "__bervenv__")
		file, err := os.Stat(tryBervenv)
		if err == nil && file.IsDir() {
			return tryBervenv, nil
		}
		parentDir := fp.Dir(currPath)
		if parentDir == currPath {
			break
		}
		currPath = parentDir
	}

	return "", fmt.Errorf("__bervenv__ not found in current or parent directories")
}
