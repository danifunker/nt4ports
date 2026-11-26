#!/bin/bash

# FileZilla language file generation script
# Copyright 2003 by Tim Kosse (mailto:tim.kosse@gmx.de)

# This script generates the language resource files out of resource files
# of FileZilla. Start the script and follow the instructions to generate the
# language files.

echo "Enter the name of the language (Example: English):"
read LANGNAME

echo "Enter the 2 or 3 letter abbreviation of the language name, starting with an uppercase letter (Example: En):"
read SHORTNAME

echo "Enter the Locale ID as hexadecimal number, without any prefixes (Example: 41d):"
read LCID

echo "Enter the codepage (Example: 1252):"
read CODEPAGE

echo "Enter your name:"
read NAME

echo "Enter your mail address:"
read EMAIL

LOWLANGNAME=`echo "$LANGNAME" | tr '[:upper:]' '[:lower:]'`
UPLANGNAME=`echo "$LANGNAME" | tr '[:lower:]' '[:upper:]'`

echo "Creating output dirs"
mkdir $LANGNAME
mkdir $LANGNAME/res

echo "Copying necessary files"
cp ../resource.h $LANGNAME/
cp ../res/Toolbar.bmp $LANGNAME/res/

echo "Processing project/resource files"
cat English/FzResEn.dsp | sed -e "s/<LCID>/$LCID/" -e "s/FzResEn/FzRes$SHORTNAME/" -e "s/$/\\r/" > $LANGNAME/FzRes$SHORTNAME.dsp
cat English/FzResEn.vcproj | sed -e "s/1033/$((16#$LCID))/" -e "s/FzResEn/FzRes$SHORTNAME/" -e "s/$/\\r/" > $LANGNAME/FzRes$SHORTNAME.vcproj
cat ../FileZilla.rc | sed -r -e "s/VALUE \"Comments\", \".*(\\\\0)?\"/VALUE \"Comments\", \"$LANGNAME\\\\0\"/" \
			  -e "s/VALUE \"FileDescription\", \".*(\\\\0)?\"/VALUE \"FileDescription\", \"$LANGNAME language DLL for FileZilla\\\\0\"/" \
                          -e "s/VALUE \"ProductName\", \".*(\\\\0)?\"/VALUE \"ProductName\", \"FileZilla Language DLL\\\\0\"/" \
                          -e "/^IDI_/d" \
                          -e "/^IDB_/d" \
                          -e "/^IDR_MAINFRAME.\s+ICON/d" \
                          -e "/^1.\+\(24\|RT_MANIFEST\)/d" \
                          -e "s/^LANGUAGE LANG_NEUTRAL, SUBLANG_NEUTRAL/LANGUAGE LANG_$UPLANGNAME, SUBLANG_DEFAULT/" \
                          -e "s/^#pragma code_page(.\+)/#pragma code_page($CODEPAGE)/" \
                          -e "s/<your name> (<your e-mail>)/$NAME ($EMAIL)/" \
                          -e "s/$/\\r/" \
    > $LANGNAME/FzRes$SHORTNAME.rc
cat English/fz_english.nsh | sed -e "s/^!define CURLANG \${LANG_ENGLISH}/!define CURLANG \${LANG_$UPLANGNAME}/" \
                                 -e "s/^LangString FZLanguageName \${CURLANG} \"English\"/LangString FZLanguageName \${CURLANG} \"$LANGNAME\"/" \
                                 -e "s/^;This file has been translated by <insert your name here>/;This file has been translated by $NAME (mailto:$EMAIL)/" \
                                 -e "s/$/\\r/" \
    > $LANGNAME/fz_$LOWLANGNAME.nsh
cat English/makefile.mingw | sed -e "s/FzResEng/FzRes$SHORTNAME/g" \
                                 -e "s/$/\\r/" \
    > $LANGNAME/makefile.mingw

echo "Done, the language files are now in the $LANGNAME subdirectory"