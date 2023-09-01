#!/bin/bash
#Idan Simai 206821258

#Global variables.
folderPath=$1
word=$2
flag=$3
directory=$(pwd)

#Enter the location.
cd "$folderPath" || exit

#Set a varaible this folder's list.
folders=$(ls)

#Check if enough parameters.
if [ $# -lt 2 ]
then
  echo "Not enough parameters"
  exit
fi

#If the file extension is .out, delete it.
for f in $folders
do
  if [ "${f: -4}" == ".out" ]
  then
    rm "$f"
  fi
done


#Check if the given word exists in a c file, than compile the file.
for f in $folders
do
  if [ "${f: -2}" == ".c" ] && grep -w -q -i $word "$f"
  then
    gcc "$f" "-o" "${f%%.*}.out" "-w"
  fi
done

#If the recursive flag is set, check recursivley.
if [[ "$flag" == "-r" ]]
then
  #Enter the current directory so the recursion works.
  cd "$directory" || exit
  for f in $folders
  do
    if [[ -d "$folderPath/$f" ]]
    then
      ($0 "$folderPath/$f" "$word" "-r")
    fi
  done
fi