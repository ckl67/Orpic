import time

print("\nList --> A list is a collection which is ordered and changeable.")
print("==================================================================")
thislist = ["apple", "banana", "cherry"]
print(thislist[1])
print(thislist[0:2])

print("\nTuple --> A tuple is a collection which is ordered and unchangeable.")
print("======================================================================")
thistuple = ("apple", "banana", "cherry", "orange", "kiwi", "melon", "mango")
print("tuple = ", thistuple)
print(thistuple[1])
print(thistuple[2:5])

print("\n Set  --> A set is a collection which is unordered and unindexed")
print("===================================================================")
thisset = {"apple", "banana", "cherry"}
print(thisset)

print("\n Dictionnary  --> A dictionary is a collection which is unordered, changeable and indexed.")
print("============================================================================================")
thisdict = {
  "brand": "Ford",
  "model": "Mustang",
  "year": 1964
}
print(thisdict["year"])
for x in thisdict.values():
    print(x)

print("\n Dictionnary List")
print("====================")
thislistdict = [
    {"Nom": "Klugesherz",
      "Prénom": "Christian",
      "Age": 55},
    
    {"Nom": "Klugesherz",
      "Prénom": "Sandrine",
      "Age": 50},
    
    {"Nom": "Sand",
      "Prénom": "David",
      "Age": 49}
    ]

print(thislistdict[0])
print(thislistdict[0]["Prénom"])

# Iteration
print("\nItération")
for n in thislist:
    print(n)

   
# generateur
def create_generate():
    x = 0
    while x <= 100:
        yield "data:" + str(x) + "\n"
        x = x + 10
        time.sleep(0.5)


# Appeler : create_generate() n’exécute pas le code de create_generate
# create_generate() retourne un objet générateur.
generateur = create_generate()
print(generateur)


# Tant qu’on ne touche pas au générateur, il ne se passe rien.
# Puis, dès qu’on commence à "itérer" sur le générateur, le code de la fonction s’exécute.
for i in generateur:
    print(i)       
