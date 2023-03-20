from dataclasses import dataclass
import csv
import os
import re
import sys

override = {
	'dna4_count_mismatches', # deprecated
	'dna_version', # trivial
	'dnax_iupac_codes', # table
	'dnax_revcomp_table', # table
	'dnax_to_dna4_table', # table
}


def dnaSymbols(symbols, rx=re.compile(r"^dna[4x]?_\w+$")):
	return symbols.__class__(symbol for symbol in symbols if rx.match(symbol) is not None)


def loadExportedSymbols(filename):
	with open(filename) as file:
		linereader = csv.reader(file, delimiter=' ')
		return dnaSymbols({line for (*_, line) in linereader})


def loadManPages(directory):
	allfiles = os.listdir(directory)
	return {file[:-2] for file in allfiles if file.endswith('.3')}


def loadDocs(directory):
	allfiles = os.listdir(directory)
	return {file[:-5] for file in allfiles if file.endswith('.3.md')}


def loadCapi(filename):
	ret = set()
	identifier = re.compile(r"\w+")
	with open(filename) as file:
		for line in file:
			ret |= set(identifier.findall(line))
	return dnaSymbols(ret)


def loadInstalledManPages(mesonBuildFile):
	with open(mesonBuildFile) as file:
		pass


def formatRow(symbol, values):
	vals = (f"{str(value):<8}" for value in values)
	tmp = " ".join(vals)
	return f"{symbol:<30}\t{tmp} {'override' if symbol in override else ''}"


def contain(sets, value):
	return sets.__class__(value in s for s in sets)


def makeTable(sets):
	symbols = set().union(*sets)
	return sorted((symbol, contain(sets, symbol)) for symbol in symbols)


def printTable(table):
	blank = " " * 30
	rows = (formatRow(*row) for row in table)

	print(f"{blank}\texport   manpage  docs     capi     cppapi")
	for row in rows:
		print(row)


def validateTable(table):
	def validRow(symbol, values):
		return all(values) or symbol in override
	return all(validRow(*row) for row in table)


def main(topdir):
	exportedSymbols = loadExportedSymbols(f"{topdir}/exportedsymbols")
	manPages = loadManPages(f"{topdir}/man")
	docs = loadDocs(f"{topdir}/docs")
	capi = loadCapi(f"{topdir}/include/dna.h")
	cppapi = loadCapi(f"{topdir}/include/dna.hpp")

	table = makeTable((exportedSymbols, manPages, docs, capi, cppapi))
	printTable(table)
	valid = validateTable(table)
	sys.exit(0 if valid else 1)


if __name__ == '__main__':
	main(sys.argv[1])
