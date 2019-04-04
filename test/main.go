package main

import (
	"fmt"
	"time"
)

func main() {
	name, offset := time.Now().In(time.Local).Zone()
	fmt.Printf("%s +%dh\n", name, (offset/60)/60)
}
