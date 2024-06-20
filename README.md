minesweeper z użyciem bibliotek ncurses i argparse  

```
sterowanie:  
    strzałki/WASD       poruszanie kursorem  
    spacja              odkrycie zaznaczonego pola  
    f                   oflagowanie pola  
    q                   zakończenie programu  
```

```
$ ./minesweeper -h  
Usage: minesweeper [--help] [--version] [--rows ROWS] [--cols COLS]  
                   [--mines MINES] [--difficulty D]  

Optional arguments:  
  -h, --help          shows help message and exits  
  -v, --version       prints version information and exits  
  -r, --rows ROWS     board height [default: 9]  
  -c, --cols COLS     board width [default: 9]  
  -m, --mines MINES   mine count [default: 10]  
  -d, --difficulty D  difficulty level (1-3) [default: 1]  
```

![screenshot.png](screenshot.png)