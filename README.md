# MyCLC -- My Command-line Lisp Calculator

### Intro
This project is an overhaul of a past project of mine (which I still may come back to), Junior. Junior is an uncomplete, mini (junior) Lisp dialect, aiming for general Unix-like scripting. At this point in Junior's development, the software is a glorified Lisp calculator... And then I had an epiphany; if I took Junior, and improved on it's calculating functionality, I could create my own calculator for terminal calculations (drive-by-keyboard ftw). Thus, MyCLC.

### Why
A command-line calculator because I hate `expr`. Really, I always mess up when using it. Also, I think Lisp is a lovely family of languages. Harnessing the power of Lisp to to accomplish the task of general calculation, done in the terminal... Brilliance.

### TODO
For one, I need to improve this README document, as well as clean up and/or refactor some of the source code. I will be using this README as the primary form of documentation. Lastly, I'd like to include an `-e` flag for the script so that I can directly execute a one-liner, eg:
```
myclc -e (+ (/ 100 10) (10))
20
```

### DISCLAIMER
This will be needing further testing and improving, not to mention a need for more functionality. Harnessing C libraries that are generally found on GNU/Linux boxes will be of great use in improving the functionality.

### LICENSE
Proudly licensed under the GNU GPLv2. Love your Free/Libre software.
