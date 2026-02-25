/*
The obvious comments are from the cobra-cli tool which generated scaffolding, not an AI
*/

package cmd

import (
	"encoding/json"
	"errors"
	"fmt"
	"os"
	"path"
	"slices"

	"github.com/arvillacl16-bit/mineraloil/helpers"
	"github.com/spf13/cobra"
)

type Dependency struct {
	Name     string
	Versions []string
}

type ArchOS struct {
	OS           string `json:"os"`
	Architecture string `json:"architecture"`
}

type PackageMetadata struct {
	Version            string       `json:"version"`
	BerylliumStandards []string     `json:"beryllium_standards"`
	Dependencies       []Dependency `json:"dependencies"`
	Description        string       `json:"description"`
	ArchOSes           []ArchOS     `json:"arch_os"`
}

// queryCmd represents the query command
var queryCmd = &cobra.Command{
	Use:   "query",
	Short: "Queries a package/packages",
	Args:  cobra.MinimumNArgs(1),
	RunE: func(cmd *cobra.Command, args []string) error {
		for _, pack := range args {
			bervenv, err := helpers.FindBervenv()
			if err != nil {
				return err
			}
			jsonPath := path.Join(bervenv, "syspacks", pack, "pkg.json")
			buffer, err := os.ReadFile(jsonPath)
			if err != nil {
				return fmt.Errorf("Failed to read %s: %w", jsonPath, err)
			}

			var pkg PackageMetadata
			if err := json.Unmarshal(buffer, &pkg); err != nil {
				return fmt.Errorf("Invalid json in %s: %w", pack, err)
			}

			if pkg.Version == "" || len(pkg.BerylliumStandards) == 0 || len(pkg.Dependencies) == 0 {
				return errors.New("Malformed pkg.json")
			}

			fmt.Printf("Package %s\n", pack)
			if pkg.Description != "" {
				fmt.Printf("%s\n", pkg.Description)
			}

			fmt.Printf("Version: %s\n", pkg.Version)
			fmt.Println("Beryllium Standards: ")
			for i := 0; i < len(pkg.BerylliumStandards); i += 2 {
				if i+1 < len(pkg.BerylliumStandards) {
					fmt.Printf("    %s-%s\n", pkg.BerylliumStandards[i], pkg.BerylliumStandards[i+1])
				}
			}

			fmt.Println("Dependencies: ")
			for _, dep := range pkg.Dependencies {
				fmt.Printf("    %s:\n", dep.Name)
				for i := 0; i < len(dep.Versions); i += 2 {
					if i+1 < len(dep.Versions) {
						fmt.Printf("        %s-%s\n", dep.Versions[i], dep.Versions[i+1])
					}
				}
			}

			if pkg.ArchOSes != nil {
				validOSes := [...]string{"macOS", "windows", "linux"}
				validArchitectures := [...]string{"ARM", "x86", "x86-64", "ARM64"}
				fmt.Println("Supported systems: ")
				for _, archOS := range pkg.ArchOSes {
					if !(slices.Contains(validOSes[:], archOS.OS) && slices.Contains(validArchitectures[:], archOS.Architecture)) {
						return errors.New("Malformed pkg.json")
					}

					fmt.Printf("    OS: %s, Architecture: %s\n", archOS.OS, archOS.Architecture)
				}
			}
		}
		return nil
	},
}

func init() {
	rootCmd.AddCommand(queryCmd)

	// Here you will define your flags and configuration settings.

	// Cobra supports Persistent Flags which will work for this command
	// and all subcommands, e.g.:
	// queryCmd.PersistentFlags().String("foo", "", "A help for foo")

	// Cobra supports local flags which will only run when this command
	// is called directly, e.g.:
	// queryCmd.Flags().BoolP("toggle", "t", false, "Help message for toggle")
}
