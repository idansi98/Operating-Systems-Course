#!/bin/bash
#Idan Simai 206821258

player1Pts=50
player2Pts=50
player1Input=0
player2Input=0
ballPosition=0
input=0
gameStateIndicator=0

#Print the top area of the board.
function printTop() {
  echo " Player 1: ${player1Pts}         Player 2: ${player2Pts} "
  echo " --------------------------------- "
  echo " |       |       #       |       | "
  echo " |       |       #       |       | "
}

#Print the bottom area of the board.
function printBottom() {
  echo " |       |       #       |       | "
  echo " |       |       #       |       | "
  echo " --------------------------------- "
  echo -e "       Player 1 played: ${player1Input}\n       Player 2 played: ${player2Input}\n\n"
}

#Print the middle area of the board using switch case.
function printMiddle() {
  case $ballPosition in
  3)
    echo " |       |       #       |       |O"
    ;;
  2)
    echo " |       |       #       |   O   | "
    ;;
  1)
    echo " |       |       #   O   |       | "
    ;;
  0)
    echo " |       |       O       |       | "
    ;;
  -1)
    echo " |       |   O   #       |       | "
    ;;
  -2)
    echo " |   O   |       #       |       | "
    ;;
  -3)
    echo "O|       |       #       |       | "
    ;;
  esac
}

#This function sets the the updated points to player 1.
function setPlayer1Pts() {
  player1Pts=$(( $player1Pts - $player1Input ))
}

#This function sets the the updated points to player 2.
function setPlayer2Pts() {
  player2Pts=$(( $player2Pts - $player2Input ))
}

#This function sets the the updated ball's position.
function setBallPosition() {
  if [ $player1Input -gt $player2Input ]
  then
    if [ $ballPosition -le 0 ]
    then
        ballPosition=1
    else
      ballPosition=$(( $ballPosition + 1 ))
    fi
  fi
  if [ $player2Input -gt $player1Input ]
  then
    if [ $ballPosition -ge 0 ]
    then
      ballPosition=-1
    else
      ballPosition=$(( $ballPosition - 1 ))
    fi
  fi
}

#Function to check the validation of the entered number.
function ifValidPlayer1() {
  input=$player1Input
  if  isANumber && [ $player1Input  -le  $player1Pts ]
  then
    return 0
  else
    echo "NOT A VALID MOVE !"
    player1Turn
    return 1
  fi
}

#Play 1 turn.
function player1Turn() {
  echo "PLAYER 1 PICK A NUMBER: "
  read -s player1Input
  if ifValidPlayer1
  then
    setPlayer1Pts
  fi
}

#Play 1 turn.
function player2Turn() {
  echo "PLAYER 2 PICK A NUMBER: "
  read -s player2Input
  if ifValidPlayer2
  then
    setPlayer2Pts
  fi
}

#Function to check the validation of the entered number.
function ifValidPlayer2() {
  input=$player2Input
  if  isANumber  && [ $player2Input  -le  $player2Pts ]
  then
    return 0
  else
    echo "NOT A VALID MOVE !"
    player2Turn
    return 1
  fi
}

#Function to check if the entered input is a number.
function isANumber() {
  if [[ $input =~ ^[0-9]+$ ]]
  then
    return 0
  else
    return 1
  fi
}

#Function to set and print the game state.
function gameState() {
  if [ $ballPosition -eq -3 ]
  then
    echo "PLAYER 1 WINS !"
    gameStateIndicator=1
  elif [ $player2Pts -eq 0 ] && [ $player1Pts -gt 0 ]
  then
    echo "PLAYER 1 WINS !"
    gameStateIndicator=1
  elif [ $player2Pts -eq 0 ] && [ $player1Pts -eq 0 ] && [ $ballPosition -gt 0 ]
  then
    echo "PLAYER 1 WINS !"
    gameStateIndicator=1
  elif [ $ballPosition -eq 3 ]
  then
    echo "PLAYER 2 WINS !"
    gameStateIndicator=2
  elif [ $player1Pts -eq 0 ] && [ $player2Pts -gt 0 ]
  then
    echo "PLAYER 2 WINS !"
    gameStateIndicator=2
  elif [ $player1Pts -eq 0 ] && [ $player2Pts -eq 0 ] && [ $ballPosition -lt 0 ]
  then
    echo "PLAYER 2 WINS !"
    gameStateIndicator=2
  elif [ $player1Pts -eq 0 ] && [ $player2Pts -eq 0 ] && [ $ballPosition -eq 0 ]
  then
    echo "IT'S A DRAW !"
    gameStateIndicator=3
  else
    true
  fi
}

#Function to make one move of the game.
function makeOneMove() {
  player1Turn
  player2Turn
  setBallPosition
  printTop
  printMiddle
  printBottom
  gameState
}

#Function to run the game using the loop.
function gameLoop() {
  while [ $gameStateIndicator -eq 0 ]
  do
    makeOneMove
  done
}

#Function to the start the game.
function startTheGame() {
  printTop
  echo " |       |       O       |       | "
  echo " |       |       #       |       | "
  echo " |       |       #       |       | "
  echo " --------------------------------- "
  gameLoop
  exit
}

startTheGame