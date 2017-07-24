-- Test for embeding
print "We can do some initialisation here if we want"
keys = { -- A bit strange way to do it but fast enough now.
   [1073741906] = "up"; -- Up Arrow
   [1073741905] = "down"; -- Down -""-
   [1073741904] = "left"; -- Left -""-
   [1073741903] = "right"; -- Right -""-
}

--a = mysin(2.2) -- Call a C function
--print(a)

-- function test(a, b)
--    return a + b
-- end

function on_keydown(symbol)
   if keys[symbol] ~= nil then
      local key = keys[symbol] -- No need to look this up all over the place

      if key == "up" then
         -- y - 2
      elseif key == "down" then
         -- y + 2
      elseif key == "left" then
         -- x - 2
      elseif key == "right" then
         -- x + 2
      else
         print("Unknown key " .. symbol)
      end
   end
end
