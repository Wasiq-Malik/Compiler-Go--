func Integer: numPrint (Integer: num, Integer: length)
{
	num := 1000;
	length := 5;

	Integer: i, j, first, temp;
	Integer : c;

	temp := 999;
	c := 20 + num * (length + 1); /* arithmatic expression */
	println(c);

	In >> i;
	println(i);

	while i > 0 :
	{
		/* this is a comment */
		i:= i - 1;

		if i = 1:{

			println(100);

		}
		elif i = 2:{
			println(200);
		}
		else
		{
			println(i);
		}

		/*This is a
		Multiline
		Comment*/

	}

	println(temp);
	ret i;
}