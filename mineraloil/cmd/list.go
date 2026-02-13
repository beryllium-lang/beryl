package cmd

import (
	"fmt"
	"os"

	"github.com/spf13/cobra"
)

func readDirectories(dirPath string) ([]string, error) {
	folders := []string{}
	entries, err := os.ReadDir(dirPath)
	if err != nil {
		return nil, err
	}

	for _, entry := range entries {
		if entry.IsDir() {
			folders = append(folders, entry.Name())
		}
	}

	return folders, nil
}

// listCmd represents the list command
var listCmd = &cobra.Command{
	Use:   "list",
	Short: "Lists all the installed packages",
	Args:  cobra.NoArgs,
	RunE: func(cmd *cobra.Command, args []string) error {
		directoryPath := "__bervenv__/syspacks"
		packages, err := readDirectories(directoryPath)
		if err != nil {
			return fmt.Errorf("failed to list packages in %s: %w", directoryPath, err)
		}

		if len(packages) == 0 {
			fmt.Println("No packages installed")
			return nil
		}

		for _, pack := range packages {
			fmt.Println(pack)
		}
		return nil
	},
}

func init() {
	rootCmd.AddCommand(listCmd)

	// Here you will define your flags and configuration settings.

	// Cobra supports Persistent Flags which will work for this command
	// and all subcommands, e.g.:
	// listCmd.PersistentFlags().String("foo", "", "A help for foo")

	// Cobra supports local flags which will only run when this command
	// is called directly, e.g.:
	// listCmd.Flags().BoolP("toggle", "t", false, "Help message for toggle")
}
