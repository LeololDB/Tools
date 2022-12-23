This is a simple test program.
```
			+=================+
			| Examples of use |
Tabe of contents	+=================+
↑ (0) Versioning [Day] [Month] [Year]
| (1) Versioning [Period] [Ver] [Destination] ([VerFile])
| (2) Versioning [SubVersion] ([VerFile])
+-[Legend]--------------------------------------------------------------+
| [Value]		- Replaced by something else, like in English,	|
|			[that] may be replaced by a sentence.		|
| ([Optionnal Value])	- Same as a Value but optionnal.		|
+-----------------------------------------------------------------------+

+-(0)-----------------------------+--------------+----------------------+
| Versioning [Day] [Month] [Year] |		 | Versioning 19 5 1973 |
+---------------------------------+		 +----------------------+
| Gives the day time-stamp using the given date, here 19/05/1973.	|
| It's important to use spaces. 					|
+-----------------------------------------------------------------------+

+-(1)-------------------------------------------------------------------+
| Versioning [Period] [Ver] [Destination] ([VerFile])			|
| Versioning 0 1234 OldFolder ./Stats/Ver	    Do you get the joke?|
+-----------------------------------------------------------------------+
| Create a directory and print its name to stdout.			|
| The name is: the current version calculated from the number given.	|
|	(For this example: 1234 is given)				|
|	That number is given by the first example, by putting the	|
|	creation date.							|
| Then with the file ./Stats/Ver (created automatically),		|
|  it deduces the sub-versioning.					|
+-[Return value]--------------------------------------------------------+
| The return value is 0 when there are no errors.			|
| Else the stdout is used to show the error. (Yes this is uncommon.)	|
+-[Example]-------------------------------------------------------------+
| Version Example: 0.1.2.3.45						|
|	0th period (big-version)					|
|	1 year								|
|	2 month								|
|	3 days								|
|	45th sub-version (starting from 0th)				|
| Result: OldFolder/1.2.3.45/ is created.				|
+-----------------------------------------------------------------------+

+-(2)---------------------------------+-----+---------------------------+
| Versioning [SubVersion] ([VerFile]) |	    | Versioning 45 ./Stats/Ver |
+-------------------------------------+	    +---------------------------+
| Gives the day time-stamp using the given date, here 19/05/1973.	|
| It's important to use spaces. 					|
+-----------------------------------------------------------------------+
```
Lebray and many other files are required for the comile to succeed.
