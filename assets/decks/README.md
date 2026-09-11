# Card decks

Each deck lives in its own directory here and carries a `deck.json` that names
it, gives the file pattern, the aspect ratio and the credit line
(`docs/design.md` §10):

    assets/decks/
      iug1904/   card-*.jpg  back.jpg  deck.json  LICENSE
      clean54/   card-*.png  back.png  deck.json

Both decks are produced by the pipeline in `tools/assets/` in milestone M8;
this directory is empty until then. The Sailfish package installs it to
`/usr/share/harbour-tarock/assets/decks`, the Android build compiles it into
the binary as a Qt resource, and `Paths::data()` hides the difference.
