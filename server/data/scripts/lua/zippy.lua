function DIALOG()

NODE(0)
 SAY("Hi. Please choose, time is money!")
 ANSWER("Give me 100000 NC!",2)
 ANSWER("Take 100000 NC!",3)

NODE(2)
 GIVEMONEY(100000)
 SAY("Here you go, have fun")
 ENDDIALOG()

NODE(3)
 TAKEMONEY(100000)
 SAY("Cool, thanks :)")
 ENDDIALOG()

end