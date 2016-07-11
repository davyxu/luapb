


if TestBytes(1, Encode("tutorial.Person", 
{
	name = "hello",
	id = 123456,
	
	test = {1, 2},	
	
}) ) then
	print("test 1 ok")
end