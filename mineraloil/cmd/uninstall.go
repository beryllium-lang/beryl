package cmd

import (
	"fmt"
	"os"
	"path"

	"github.com/spf13/cobra"
)

func Ask() bool {
	fmt.Printf("Are you sure? ")
	var c rune
	fmt.Scan(&c)
	if c == 'Y' || c == 'y' {
		return true
	}
	return false
}

func IsDir(path string) (bool, error) {
	fileInfo, err := os.Stat(path)
	if err != nil {
		return false, err
	}

	return fileInfo.IsDir(), nil
}

// uninstallCmd represents the uninstall command
var uninstallCmd = &cobra.Command{
	Use:   "uninstall",
	Short: "Uninstalls packages",
	Args:  cobra.MinimumNArgs(1),
	RunE: func(cmd *cobra.Command, args []string) error {
		fmt.Println("uninstall called")
		if cmd.Flags().Lookup("force").Changed {
			force = true
		}

		doUninstall := false || force
		if !doUninstall {
			doUninstall = Ask()
		}

		for _, pack := range args {
			packPath := path.Join("./__bervenv__", "syspacks", pack)
			isDir, err := IsDir(packPath)
			if err != nil {
				return err
			}
			if !isDir {
				return fmt.Errorf("Package %s not found", pack)
			}
			err = os.RemoveAll(packPath)
			if err != nil {
				return err
			}
			fmt.Printf("Uninstalled package %s\n", pack)
		}

		return nil
	},
}

var force bool

func init() {
	uninstallCmd.PersistentFlags().BoolVarP(&force, "force", "f", false, "Stops the confirmation prompt")
	rootCmd.AddCommand(uninstallCmd)

	// Here you will define your flags and configuration settings.

	// Cobra supports Persistent Flags which will work for this command
	// and all subcommands, e.g.:
	// uninstallCmd.PersistentFlags().String("foo", "", "A help for foo")

	// Cobra supports local flags which will only run when this command
	// is called directly, e.g.:
	// uninstallCmd.Flags().BoolP("toggle", "t", false, "Help message for toggle")
}
