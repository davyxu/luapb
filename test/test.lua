
luapb_register("game.pb")

--[[

if TestEncode(1, Encode("tutorial.Person", 
{
	name = "hello",
	id = 123456,
	
	test = {1, 2},	

	phone = {
	
		{number= "789", type = "WORK" },
		
		{number= "456" , type = "HOME" },
	
	},
	
}) ) then
	print("test 1 ok")
end
]]

local buff = luapb_encode("tutorial.Person", 
{
	name = "hello",
	test = {1, 2},	
	
	phone = {
	
		{number= "789", type = "WORK" },
		
		{number= "456" , type = "HOME" },
	
	},
})

function dump( t )  
    local print_r_cache={}
    local function sub_print_r(t,indent)
        if (print_r_cache[tostring(t)]) then
            print(indent.."*"..tostring(t))
        else
            print_r_cache[tostring(t)]=true
            if (type(t)=="table") then
                for pos,val in pairs(t) do
                    if (type(val)=="table") then
                        print(indent.."["..pos.."] => "..tostring(t).." {")
                        sub_print_r(val,indent..string.rep(" ",string.len(pos)+8))
                        print(indent..string.rep(" ",string.len(pos)+6).."}")
                    elseif (type(val)=="string") then
                        print(indent.."["..pos..'] => "'..val..'"')
                    else
                        print(indent.."["..pos.."] => "..tostring(val))
                    end
                end
            else
                print(indent..tostring(t))
            end
        end
    end
    if (type(t)=="table") then
        print(tostring(t).." {")
        sub_print_r(t,"  ")
        print("}")
    else
        sub_print_r(t,"  ")
    end
    print()
end


dump( luapb_decode( "tutorial.Person", buff ) )

--[[
hello×Ö¶Î = 1+ 6 = 7
tag= varient_size(1) = 1
value= 5[len(hello)] + varient_size(5) = 6


number×Ö¶Î = 1+4 = 5
tag=varient_size(1) = 1
value=3[len(789)] + varient_size(3) = 4

phone×Ö¶Î=number×Ö¶Î + tag[varient_size(1)] = 6
]]