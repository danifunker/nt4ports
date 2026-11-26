#!/bin/bash


if [ "$1" == "" ]
then
  echo This script fixes the resource files so that they can be compiled using MinGW.
  echo This script awaits a ressource file as a parameter.
elif [ ! -e $1 ]
then
  echo Error, input file not found.
else
  cat $1 | sed -r -f fixmingw.sed > out.tmp
  cat out.tmp | sed -e "s/$/\r/" > $1
  rm out.tmp
fi