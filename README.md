# Pracenje-drumskog-saobracaja

Aplikacija koja na osnovu snimaka sa kamera iznad autoputa, prati, prepoznaje i prebrojava broj vozila koja su u odredjenom vremenskom periodu prosla.

Pisana u C++ jeziku. Primenom OpenCV biblioteke.

**Autori**

+ Student: Stevan Kosijer (RA128/2013) [@stkosijer](https://github.com/skosijer)
+ Student: Filip Dušić (RA126/2013) 
+ Grupa: 9
+ Asistent: Stefan Andjelić
+ Repozitorijum: [@Pracenje-drumskog-saobracaja](https://github.com/skosijer/Pracenje-drumskog-saobracaja)

![alt text](https://github.com/skosijer/Pracenje-drumskog-saobracaja/blob/master/cartracking.png)

# Problem koji se rešava

Primena tehnika računarske inteligencije u praktičnim domenima razvojem odgovarajuće hardverske podrške postaje sve više uobičajena. Jedna od takvih oblasti je i computer vision, gde se složenost problema za koje se nudi rešenje sve više povećava. Oblast koja izaziva dosta pažnje svojim dostignućima, ali i mogućim implikacijama za čovečanstvo jeste razvoj autonomnih sistema, pre svega razvoj sistema za autonomno upravljanje i asistenciju u saobraćaju.

# Opis problema

Podzadatak takvih sistema jeste prepoznavanje okruženja u kojem se nalaze kako bi mogli donositi adekvatne odluke kako bi obezbedili pravilno, bezbedno i funkcionisanje u skladu sa zakonom i propisima. Fokus ovog projekta jeste istraživanje algoritama za prepoznavanje učesnika u saobraćaju sa ulaznog medijuma (video snimak). Na ulaznim podacima je potrebno obeležiti pojave saobraćajnih vozila, a zatim interpretirati podatke i izvršiti prepoznavanje kao i prebrojavanje onih koja prelaze određenu granicu.

# Cilj projekta

Cilj projekta je istraživanje i upoređivanje efikasnosti algoritama u fazi detekcije samih vozila, kako bi se ostvarila što veća praktična izvodljivost u realnom uslovima.

# Metodologija rešavanja

U zavisnosti od izbora ulaznih podataka (video), metodologija rešavanja prati sledeće korake:

- učitavanje ulaznih podataka u sliku
- obrada slike/videa
- detekcija vozila
- izdvajanje regiona od interesa
- obrada regiona od interesa
- klasifikacija regiona od interesa
- praćenje pozicije
- beleženje i direktno prikazivanje rezultata u realnom vremenu

# Podaci

Video zapisi sa autoputeva.

# Algoritmi

Radi obrade videa biće upotrebljeni algoritmi iz soft computing i computer vision domena: morfološke operacije, izdvajanje regiona, podešavanje thresholda, transformacija slike, segmentacija slike, praćenje i izdvajanja objekata od interesa.

# Metrika za poređenje performansi algoritama

Algoritmi će se porediti u odnosu na potrebno vreme obrade podataka i njihovu preciznost, kao dve glavne komponente koje figurišu u problemu prepoznavanja saobraćajnih vozila.

# Validacija rešenja

Validacija će se vršiti na osnovu ukupne tačnosti obrade jednog snimka, broja prepoznatih vozila i ultimativno poređenjem različitih snimaka i efikasnosti algoritma za različite snimke.


