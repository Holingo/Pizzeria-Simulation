
# Projekt: Pizzeria

## 1. Wprowadzenie

W tym projekcie stworzymy system symulujący funkcjonowanie pizzerii, w której zarządzanie stolikami, klientami oraz kasą jest kluczowe do uzyskania optymalnego zysku i sprawnego działania lokalu. System będzie obejmował obsługę stolików, przyjmowanie zamówień, zarządzanie klientami (indywidualnymi oraz grupami) oraz reagowanie na sygnał o pożarze, który wymusi natychmiastowe opuszczenie lokalu.

## 2. Kluczowe założenia

- Pizzeria posiada stoliki o różnych rozmiarach: 1-osobowe, 2-osobowe, 3-osobowe oraz 4-osobowe. Liczba każdego typu stolików to X1, X2, X3, X4.
- Klienci mogą przychodzić indywidualnie lub w grupach (2-osobowe, 3-osobowe).
- Każda grupa, która przychodzi, musi usiąść przy wolnym stoliku. Jeśli stolik jest zajęty, klient musi poczekać na zwolnienie miejsca, ale nie wolno czekać z gorącą pizzą.
- Różne grupy klientów nie mogą dzielić stolika, chyba że są równoliczne (np. dwie grupy 2-osobowe mogą usiąść przy stoliku 4-osobowym).
- Po sygnale o pożarze klienci muszą natychmiast opuścić pizzerię, a kasa zostaje zamknięta.

## 3. Model Obiektów

### 3.1. Klient
- **Atrybuty**:
  - `id`: ID klienta
  - `liczba_osob_w_grupie`: liczba osób w grupie
  - `status_zamowienia`: status zamówienia (oczekuje, zjedzono)
- **Metody**:
  - `złożyć_zamówienie()`: klient składa zamówienie
  - `otrzymać_pizzę()`: klient otrzymuje pizzę
  - `opóścić_pizzerię()`: klient opuszcza pizzerię po pożarze

### 3.2. Stolik
- **Atrybuty**:
  - `numer_stolika`: numer stolika
  - `pojemnosc`: liczba osób, dla których stolik jest przeznaczony
  - `status`: status stolika (wolny, zajęty)
- **Metody**:
  - `zajmij_stolik(grupa)`: przypisuje stolik grupie
  - `zwolnij_stolik()`: zwalnia stolik po zakończeniu posiłku

### 3.3. Pizzeria
- **Atrybuty**:
  - `stoliki`: lista stolików o różnych pojemnościach (1, 2, 3, 4-osobowe)
  - `kasa`: stan kasy (otwarta, zamknięta)
- **Metody**:
  - `przyjmij_zamówienie(grupa)`: przyjmuje zamówienie od grupy klientów
  - `przydziel_stolik(grupa)`: przypisuje grupie odpowiedni stolik
  - `reaguj_na_pozar()`: reaguje na sygnał o pożarze

### 3.4. Strażak
- **Atrybuty**:
  - `id`: ID strażaka
- **Metody**:
  - `wyślij_sygnal_o_pozarze()`: wysyła sygnał o pożarze

### 3.5. Kasjer
- **Atrybuty**:
  - `stolik_zajety`: stolik, który jest obecnie zajęty
  - `kasa_otwarta`: stan kasy (czy jest otwarta)
- **Metody**:
  - `zamknij_kase()`: zamyka kasę po sygnale o pożarze
  - `zlicz_zyski()`: nalicza zarobki na podstawie zamówień

## 4. Opis Funkcjonowania

### 4.1. Zarządzanie stolikami
Pizzeria posiada stoliki o różnych rozmiarach. Klienci mogą przyjść indywidualnie lub w grupach. Pizzeria dba o to, aby klientom przypisane były odpowiednie stoliki.

- Klient o liczbie osób 2 może zająć stolik 2-osobowy lub stolik 4-osobowy (jeśli jest wolny).
- Zajęcie stolika następuje po dokonaniu zamówienia.

### 4.2. Zarządzanie zamówieniami
Klienci składają zamówienia, a po otrzymaniu pizzy mogą usiąść przy stoliku. Ważne jest, aby nie czekali z gorącą pizzą na miejsce – zamówienie musi być skojarzone z dostępnością stolika.

### 4.3. Pożar
Po sygnale o pożarze wszyscy klienci muszą natychmiast opuścić lokal. Po ich wyjściu kasa jest zamknięta.

### 4.4. Zyski i zamknięcie kasy
Po każdym zamówieniu kasjer dodaje zarobki do ogólnej puli. Po sygnale o pożarze kasa jest zamknięta.

## 5. Testy

### 5.1. Testowanie zamówienia
- Czy klient może złożyć zamówienie tylko wtedy, gdy stolik jest dostępny?
- Czy zamówienie zostaje przydzielone po zwolnieniu stolika?

### 5.2. Testowanie pożaru
- Czy po sygnale o pożarze klienci natychmiast opuszczają pizzerię?
- Czy kasa jest zamknięta po sygnale o pożarze?

### 5.3. Testowanie zysków
- Czy kasjer poprawnie nalicza zyski po każdym zamówieniu?

### 5.4. Testowanie konkurencyjności grup
- Czy różne grupy klientów (np. 2-osobowe, 3-osobowe) nie siadają przy tym samym stoliku, chyba że są równoliczne?

## 6. Podsumowanie

Projekt "Pizzeria" ma na celu stworzenie systemu do zarządzania klientami, stolikami i zamówieniami w pizzerii, z uwzględnieniem ważnych aspektów takich jak reagowanie na pożar, przypisywanie odpowiednich stolików do grup klientów oraz zamykanie kasy po zdarzeniu awaryjnym.
