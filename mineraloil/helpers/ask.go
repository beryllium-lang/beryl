package helpers

import (
	"fmt"
)

func Ask() bool {
	var c rune
	fmt.Printf("Are you sure? Y/n: ")
	fmt.Scan(&c)
	return c == 'y' || c == 'Y'
}
