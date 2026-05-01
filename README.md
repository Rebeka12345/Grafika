# Grafika-projektek leírása:
---
## Első projekt
A feladat egy olyan program és a hozzá tartozó GLSL shader fájlok elkészítése, amely egy kör megjelenítésére és animációjára képes. A kör a képernyő széleit érintve, megfelelő szögben visszapattan, miközben mozgása folyamatos marad (a lépéshossz falérintéskor arányosan módosul).

### Követelmények

#### 1. Ablak létrehozása
- A program hozzon létre egy **600×600 pixel méretű négyzetes ablakot**.
- A háttér színe legyen **sárga** (az árnyalat szabadon választható).

#### 2. Kör megjelenítése
- A képernyő közepén jelenjen meg egy:
  - **r = 50 pixel sugarú kör**
  - **zöld határvonallal**
  - **piros centrummal**
- A határ és a centrum között **lineáris színinterpoláció** legyen.
- Tipp: a kör középpontját érdemes **uniform változóként** átadni a fragment shader számára.
- A zöld és piros színek árnyalata szabadon választható.

#### 3. Kör mozgása
- A kör a képernyő közepéről induljon.
- Mozogjon **vízszintesen (x-tengely mentén)** oda-vissza.
- A képernyő szélét **pontosan érintve pattanjon vissza**.

#### 4. Vízszintes szakasz
- A képernyő közepén jelenjen meg egy:
  - **kék színű**
  - **vízszintes szakasz**
  - hossza: **az ablak oldalhosszának egyharmada**
  - vastagsága: **3 pixel**

#### 5. Felhasználói interakció
- A szakasz mozgatható legyen:
  - **Fel nyíl** → felfelé mozgatás
  - **Le nyíl** → lefelé mozgatás

#### 6. Metszésvizsgálat és színcsere
- Amikor a kör **nem metszi** a szakaszt:
  - a **határ (zöld)** és a **centrum (piros)** színei **cserélődjenek fel**
- Ez a változás az interpolált belső pontokra is hasson.

### Saját ötlet

#### Színváltoztatás
- Az **A** és **D** billentyűk segítségével legyen lehetőség a színek módosítására.

---

## Második projekt
A feladat során egy olyan alkalmazást kell elkészíteni, amely tetszőlegesen sok kontrollpont képernyőn történő elhelyezését/eltávolítását követően képes a kontrollpontoknak megfelelő fokszámú Bézier-görbe kirajzolására. A kontrollpontok a bal egérgomb nyomva tartásával elmozdíthatók, és a görbe alakja valós időben követi a változásokat.

### Követelmények

#### 1. Bézier-görbe kirajzolása
- A program használja a **4 kontrollponttal megadott harmadfokú Bézier-görbe** kirajzolását bemutató példaprogram függvényeit.
- A görbe törött vonallal legyen kirajzolva.

#### 2. Kontrollpontok megjelenítése
- A program jelenítse meg a kontrollpontokat:
  - darabszám: **3 ≤ d ≤ 9**
  - forma: **kör**
  - méret: **4 pixel átmérő**

#### 3. Kontrollpontok mozgatása
- A kontrollpontok **drag-and-drop** módszerrel mozgathatók:
  - bal egérgomb nyomva tartásával
- Mozgatás közben:
  - a görbe **valós időben frissüljön**

#### 4. Kontrollpoligon kirajzolása
- A program rajzolja ki:
  - a kontrollpontokat összekötő **kontrollpoligont**
- A poligon megjelenítése legyen folyamatosan szinkronban a pontokkal.

#### 5. Színezés
- A következő elemek különböző színnel jelenjenek meg:
  - Bézier-görbe
  - kontrollpontok
  - kontrollpoligon
- A színek **tetszőlegesen választhatók**

#### 6. Kontrollpontok kezelése
- **Bal egérgomb**:
  - üres területre kattintva → új kontrollpont létrehozása
- **Jobb egérgomb**:
  - meglévő kontrollpontra kattintva → kontrollpont törlése
- Módosítás után:
  - a görbe és minden kapcsolódó elem **újrarajzolódik**

---

## Harmadik projekt
A feladat során egy 3D jelenetet kell megvalósítani, amely több kockát és egy mozgatható kamerát tartalmaz, valamint dinamikus megvilágítással rendelkezik.

### Követelmények

#### 1. Kamera kezelése
- A kamera egy **hengerfelületen mozog** a térben.
- Beállítások:
  - kezdőpozíció: **(r, 0, 0)**, ahol **8 ≤ r ≤ 10**
  - UP vektor: **(0, 0, 1)**
  - a kamera mindig az **origóra néz**
- Mozgatás:
  - **jobb/bal nyíl** → forgás a z-tengely körül
  - **fel/le nyíl** → mozgás a z-tengely mentén
- Vetítés:
  - **perspektivikus vetítés**
  - látószög: **55°**

#### 2. Kockák elhelyezése
- A jelenet tartalmazzon **3 darab egységoldalú kockát**:
  - az első az **origó középpontjában**
  - a másik kettő:
    - a kamera nézetében az egyik **felül**, a másik **alul** helyezkedjen el
- Elhelyezés:
  - a kockák között legyen akkora távolság, hogy **éppen elférjen köztük egy további egységkocka**
- Szín:
  - minden kocka **fehér**

#### 3. Megvilágítás
- Hozzunk létre egy:
  - **diffúz fényforrást**
  - színe eltér a fehértől
- Mozgás:
  - a fényforrás **körpályán mozog** a z-tengely körül (z = 0)
  - sugár: **2 × r**
- A megjelenítés:
  - helyes **láthatósági sorrenddel** történjen
- Interakció:
  - a világítás **billentyűzettel ki- és bekapcsolható**

#### 4. Fényforrás vizualizációja
- A fényforrás jelenjen meg:
  - **gömbként**
  - sugár: **0.5**
- A gömb anyaga:
  - egyezzen meg a fényforrás színével
  - **megvilágítás nélkül jelenjen meg**

#### 5. Textúrázás
- A fényforrás gömbjének anyaga:
  - legyen kombinálva egy **Nap textúrával**
  - a textúra külső forrásból származik
