For compilation, requires SFML lib stuff to be installed.
Will require the data to be in a top level `data` folder (see scraper for data)!

Or just download the program from releases (will include current classification.json).

Controls:
```
1 = Jug
2 = Crimp
3 = Sloper
4 = Pinch
5 = Pocket
6 = Incut
7 = Edge
8 = Other
9 = Other
0 = Foothold

Left = prev
Right = next
```
NOTE: only first 5 are currently being classified, so ignore 6-0.

Format of classification.json:
```
{
	"HOLDID": "CLASSIFICATION",
	...
}
```