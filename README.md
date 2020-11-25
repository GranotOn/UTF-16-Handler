# UTF-16 Handler

- Copy an existing UTF-16 file
- Transform from Big-Endian to Little-Endian and vice-versa
- Multiple OS support (Mac, Unix, Windows)

## Usage

After using `make` you can use the following command-line arguments:
  
`source-file-name new-file-name [source-file-os-flag] [new-file-os-flag] [byte-order-flag]`

### Parameters

Name | Options
-----|-------
`<source-file-name>` | A .txt valid UTF-16 source file
`<new-file-name>` | A .txt file to be overriden with output data
`<source-file-os-flag>` | `-mac`, `-unix`, `-windows` to annotate source file OS
`<new-file-os-flag>` | `-mac`, `-unix`, `-windows` to annotate output file OS
`<byte-order-flag>` | `-swap`, `-keep` to switch Endians.

### Notes

- Providing only a source and an output file will create a copy
- Providing a `source-file-os-flag` would require also require a `new-file-os-flag`
- By default `byte-order-flag` is set to `-keep`
