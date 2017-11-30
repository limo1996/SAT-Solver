#!/bin/bash
latex report
bibtex report
latex report
latex report
dvips -t letter -o report.ps report.dvi
ps2pdf report.ps
