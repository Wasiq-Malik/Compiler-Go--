func Integer: numPrint (Integer: num, Integer: length)
{
	Integer: i, j, first, temp;
	Integer : c;

	c := 20 + num * (length + 1) - i / j; /* arithmatic expression */
	print ( "enter number" );
	In >> i;
	println (i);
	i := length;
	while i > 5 :
	{
		first:= 0;
		j := 1;
		if j = 1:{

			print("one");

			if i = 1:{
				print("nested if");
			}
			elif i = 2:{
				print("nested elif");
			}
		}
		elif j = 2:{
			print("two");
		}
		else
		{
			print("others");
		}

		/* this is a comment */
		i:= i - 1;

		/*This is a
		Multiline
		Comment*/

	}

	print("temp is");
	println(temp);
	ret i;
}