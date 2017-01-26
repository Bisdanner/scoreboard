
	; Anzeigensteuerung (Dynamisch)

	#define	_Clock	PORTD,2	
	#define	_Strobe	PORTD,0
	#define	_Data	PORTD,1
	#define	_EN	PORTD,3

	; Peripherie

	#define	L1	PORTC,0	; 6 Lampen
	#define	L2	PORTC,1
	#define	L3	PORTC,2
	#define	L4	PORTC,3
	#define	L5	PORTC,4
	#define	L6	PORTC,5
	#define Lampen	PORTC

	#define	_Tasten	PORTB	; Port für die 6 Taster

	#define	Hupe	PORTC,6	; Hupe
	#define	ExHupe	PORTC,7	; Externe Hupe

	cblock 0x1a

		ISave
		ISave1
		ISave2		; Zwischenspeicher für IRQ
		ISave3

		ZTreib		; Muster für Zeilentreiber
		Char		; Zeichencode

		Flag		; 8 Bits für verschiedene Aufgaben
				; Bit0 = Aktuelle Anzeigezeile 0 oder 1
				; Bit1 = für IRQ, 1 wenn RAM1 aktiv
				; Bit6 = 1->SetAnz aufrufen

		Work1
		Work2		; Zwischenspeicher beim einschreiben der Daten
		Work3
		Work4

		PTC
		TestCnt

		Taster		; Zwischenspeicher für die Tasterwerte

		Digit		; Digitnummer
		Col		; Aktuelle Kolonne (falsch)
		RCol		; Aktuelle Kolonne (echt aus Tabelle)
		Col1		; Zwischenspeicher für Col
		ZCntr		; Zeilenzähler für SetPDig
		CCntr		; Kolonnenzähler für SetPDig
		ZBase		; Basisadresse der aktuellen Zeile
		ZBit		; Bit welches manipuliert werden muß

		AFont
		AFont1		; Startadresse des aktuellen Fonts

		AFl
		AFl1		; Startadresse der Längentabelle
		Len		; Länge des aktuellen Zeichens

		ICount		; Byte-Zähler (Dynamisch)
		DPtr		; Zeiger auf den Data-Bereich
		SerOut		; Auszugebende Daten
		Zeile		; zu beschreibende Zeile

		RXS		; Empfangsstatus
		ZZ		; Zeichenzähler
		RXFlag		; Bit0 -> 1=Daten empfangen
				; Bit1 -> 1=Kurzer Datensatz 0=Langer Datensatz

		Check		; Speicher für Prüfsumme

		Zeichen		; Speicher für die Fontdaten eines Zeichens
		Zeichen1
		Zeichen2
		Zeichen3
		Zeichen4
		Zeichen5
		Zeichen6

		Z1_1
		Z1_2
		Z1_3
		Z2_1
		Z2_2
		Z2_3
		Z3_1
		Z3_2
		Z3_3
		Z4_1		; Anzeigepuffer für die drei einzelnen Zeichen
		Z4_2
		Z4_3
		Z5_1
		Z5_2
		Z5_3
		Z6_1
		Z6_2
		Z6_3
		Z7_1
		Z7_2
		Z7_3

		SerData		; Empfangspuffer (RS 232)

	endc


Z1		equ	D'32'	; Zeilenspeicher 1. Zeile
Z2		equ	D'52'
Z3		equ	D'72'	
Z4		equ	D'92'	; je 20 Byte lang in Bank 1
Z5		equ	D'112'
Z6		equ	D'132'
Z7		equ	D'152'	; Zeilenspeicher letzte Zeile


