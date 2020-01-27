echo "press 1 for viewing full dictionary"
echo "press 2 for searching a word"
echo "press 3 for adding"
read WORD
if [ "$WORD" = 1 ]
then
	sort test1	
elif [ "$WORD" = 2 ]
then 
	echo "Enter the word: "
	read SEARCH	
	COUNT=$(grep -i -c "$SEARCH" test1) 
	if [ "$COUNT" = 0 ]
	then 
		echo "The word does not exist in your dictionary."	
	else	
		grep -i "$SEARCH" test1
	fi
else
	echo "Enter the new word: "
	read NEW
	COUNT=$(grep -i -c "$NEW" test1) 	
	if [ "$COUNT" = 0 ]	
	then		
		echo "Enter the meaning of the word: "
		read MNG
		echo "$NEW : $MNG" >> test1
		command >> test1
		echo "The word has been successfully added to your dictionary!!"
	else
		echo "the word already exist in the dictionary"
	fi
fi
