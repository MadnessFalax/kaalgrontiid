# Thesis
## Geospatial data import/export tool for RadegastDB framework

The goal of this thesis is to implement a tool that would allow import/export of geospatial data into RadegastDB framework's sequential array memory container. 

The formates that were selected for this task are:
- [GeoJSON](https://geojson.org/)
- [KML](https://developers.google.com/kml/documentation/kmlreference)
- [OSM](https://wiki.openstreetmap.org/wiki/OSM_XML)
- [ESRI Shapefile](https://en.wikipedia.org/wiki/Shapefile)

In this work the focus is on spatial data (coordinates). Other data are not an object of this thesis.

### Parsers

The two approaches are used for data parsing. In case of binary data (Shapefile) the parser declares the structures as defined in ESRI's [Shapefile technical documentation](https://www.esri.com/content/dam/esrisites/sitecore-archive/Files/Pdfs/library/whitepapers/pdfs/shapefile.pdf). In this case the data are parsed sequentially as read from file. There's no other magic inside.

In case of text data (GeoJSON, KML, OSM) the LL(1) parser is implemented. 
For each of these formates a grammar had to be defined. 
The grammar for each of these types can be found in text files in the `.\parser\LL1\grammar\` directory. 
The grammar is then passed to parser for each formate, using interface defined for Lexer and Parser (this tool isn't able to load grammar from text file).
This can be found in respective folders in `.\parser\` directory (e.g. `.\parser\KML\` etc.). 
For each of these formates an enums are defined. 
These are used as identification for Lexer tokens, Parser rules and for custom visit logic. 
Next the Lexer patterns and Parser rules are defined. 
Lastly custom visit logic is defined.

Because of the nature of LL(1) parser (well known and simple parsing solution), the LL(1) parser can be further improved and reused for other text formates.

One of the specifications of the thesis was to not use any third party libraries (not even STL). For that reason custom containers and custom regex engine were implemented. 
The containers use interface similar to that of STL containers.

The thesis implementation uses advanced C++ principles such as SFINAE, CRTP, template specialization and other metaprogramming techniques.
There are also many leftover comments suggesting ownership of pointers passed to various methods and ctors.

### Exporters

The exporters are implemented separately for each of the formates. 
The goal was to keep their implementation as simple as possible and make them all have the same or at least very similar interface.

In case of Shapefile there are created six output files in total. 
The .shx, .shp and .dbf files are created as these are mandatory by standard. 
There are two versions. 
One for Z Shape Type and the other for regular Shape Type (for more info on shapetypes see [technical reference](https://www.esri.com/content/dam/esrisites/sitecore-archive/Files/Pdfs/library/whitepapers/pdfs/shapefile.pdf)).
Since one file can only consist of one Shape Type, you need to initialize the exporter for each exported type. 
The type is determined by template argument.

## Requirements
No third party libraries are used. The RadegastDB framework source files are needed as some of them are included in solution. The implementation should be built using MSVC compiler and targets x64 platform.
Some of the STL headers are being used (`type_traits`, `stdlib` or `stdio`). The `crtdbg` header is being used in debug mode in order to detect memory leaks.

## Examples

The three example files are included in the repository:
- Example1.cpp - full functionality demonstration (import, export, import of exported data) 
- Example2.cpp - import/export of selected formate with print out of data
- Example3.cpp - import/export of selected formate with time measurements

The fastest way to switch between the active examples is to include the desired one in project and exclude the others.

### Example #1
In this example the data are imported from one file of each formate. 
These are all loaded into single sequential array. 
The resulting data are then exported into each of the formates.
The exported data are then imported back into the sequential array to show that the data validity doesn't change through the process. 
Only non-spatial data are scrapped. I suggest running build with Release configuration.

### Example #2
In this example you should choose the formate you want to import/export.
In order to do so you need to choose the right configuration.
I suggest using Release_xxx configurations as these do not print any other debug information into console.
The data are imported and printed before insert into sequential array. They are then again printed before export.

### Example #3
In this example you should choose the formate you want to import/export.
In order to do so you need to choose the right configuration.
I suggest using Release_xxx configurations as these do not print any other debug information into console.
The data are imported and exported in the same way as in example #2. 
In this scenario the data are not printed as this would skew the time measurements.
Instead the time measurements are printed to console after each the import and export.

## Suggestions
When testing the data export, several web applications, that can load and visualise the data, were used. You can use these to check the results yourself.
These are:
- [geojson.io](https://geojson.io/) (GeoJSON)
- [kmzview.com](https://kmzview.com/) (KML)
- [mapshaper.org](https://mapshaper.org/) (Shapefile)

🐱‍👤👳🏾‍♂️🧙‍♂️ 🔥🐲 💎