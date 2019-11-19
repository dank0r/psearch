# psearch
psearch is a tool for parallel searching in your files.
## Installation
Clone this repository to your computer:
```bash
git clone https://github.com/dank0r/psearch
cd psearch
```
Compile using
```bash
make
```
## Usage
##### Required argument:
`PATTERN` - this substring will be searched.
##### Optional arguments:
`\t#` - Search using # threads. If not specified, 1 thread is used.
`-n` - If flag set, search will occur only in specified directory. Otherwise, search will happen recursively among the subfolders.
`PATH` - Path to the folder where `PATTERN` should be searched. If not specified, current folder is used.
For example:
```bash
./psearch -t4 FILE /usr/include
```
Will search for pattern FILE in /usr/include and deeper using 4 threads.
