# nativewriter
Simple native host for Chrome extensions, that easily allows to write some data from Chrome extensions to file


It accepts json commands from stdin (one command per launch)
Json command fomat: { command: "write", file: request.fname, data: request.data }; 
 command - what to do. fo now the only implemented command is "write" that appends file
 file - filename (with path)
 data - what to write

author Nikolay Safronov bfishh@gmail.com
