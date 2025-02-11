## Dependency

## build & installation 

```
gcc playontermux.c -o playontermux `pkg-config --cflags --libs gtk+-3.0`
chmod +x playontermux
mv playontermux $PREFIX/bin/
```
## Desktop entry
```
mkdir -p $HOME/.local/share/applications
mv *.desktop $HOME/.local/share/applications

update-desktop-database ~/.local/share/applications
```
