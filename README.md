# SANE IN THE MEMBRANE

Project aims to solve a very specific problems for using [SANE](https://www.sane-project.org/)
compatible scanners over the network on clients with different operating systems.
Clients should be able to scan without any drivers only depending on the server
to know how to interact with scanners.

This project creates a server binary that interacts with the scanners
(needs to be installed on a SANE compatible host) and a client application
that can talk to the server over gRPC and can request scans from the server.
Server binary should be run on a linux host with sane backends installed.
Server dynamically links to `libsane` and depends on it.

Client binary is a QT6 application that tries to connect to a server
over gRPC. It can request a list of scanners from the server and initiate
a scan. Currently all data transferred from the server to client is
uncompressed and unencrypted.
Client supports previewing scans and exporting scanned pages into a pdf document.

## TODO

Features, tasks and ideas to do in no particular order:

- [ ] Make UI nicer
- [ ] Reorder displayed pages
- [ ] Rotate pages
- [ ] Export in different formats (e.g. export only images)
- [ ] Compress packets
- [ ] Automatic server discovery
- [ ] Test windows build
- [ ] Add OCR support (probably embed or require external installation of [OCRMyPdf](https://github.com/ocrmypdf/OCRmyPDF))
