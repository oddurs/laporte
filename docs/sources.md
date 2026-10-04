# The ledger

Every figure a check is judged against, and the source it came from. See
CLAUDE.md, house rule 6. Nothing enters here unless somebody fetched the
source; a figure that is commonly quoted and cannot be traced is entered as
**unsourced**.

Entry format:

    ## S00 — what the figure is
    - figure:   value and units
    - source:   title, author or issuing body, date, section or page
    - url:      a URL that was actually fetched
    - quote:    the sentence it came from
    - status:   sourced | unsourced

Entries add a `note` (what kind of fetch stands behind it) and a `caveat`
(where the year or the scope is wrong for us) when they need them.

## How to read this ledger

Two kinds of fetch stand behind the `sourced` entries, and they are not equally
trustworthy.

- **Primary, read in full text.** The PDF was downloaded and its text read
  directly: *Notes on Distance Dialing* (AT&T, 1956), Weaver and Newell in the
  *Bell System Technical Journal* (1954), Bell System Practices sections
  AB22.066.1 (1952) and 040-011-712 (1965), the *BSTJ* of April 1978, the
  *Lenkurt Demodulator* of June 1968, and NIST Handbook 100 (1966). The scans are
  OCR'd; a quote joins hyphenated line breaks and changes nothing else.
- **Secondary, read through a fetch tool.** Wikipedia and a few web pages. The
  quote is the sentence the fetch tool returned from the page, not text read off
  the raw page. Treat these as pointers to a primary source not yet found. A
  check judged against one of them says so.

Two warnings that apply to the whole ledger.

1. **The year matters, and much of the ledger is not 1965.** The era is a Bell
   System step-by-step office about 1965 (item 0009). Where a source is from
   another year the entry says so under `caveat`. Several 1978 figures stand in
   for a 1965 one; none has been shown to differ in 1965, and none has been
   shown not to.
2. **The Precise Tone Plan was not in service in a 1965 step-by-step office.**
   See S16 and S17. The tones of the 1965 office are the older, non-precise
   ones. The Precise Tone Plan figures (S15) are here because they are the
   numbers everyone quotes, and a check must not be fooled by them.

Totals are at the foot of the file.

---

## S01 — subscriber-line battery voltage and polarity
- figure:   48 V nominal between tip and ring; tip near ground, ring at -48 V relative to tip.
- source:   "Tip and ring", Wikipedia (secondary)
- url:      https://en.wikipedia.org/wiki/Tip_and_ring
- quote:    "The voltage at a subscriber's network interface is typically 48 V between the ring and tip wires, with tip near ground (slightly negative relative to ground) and ring at −48 V relative to tip."
- status:   sourced
- note:     Secondary. Corroborated for the office (not the line) by S02. The same page also says "The nominal battery (_system_) voltage is 52.1 V, based on a 24-cell lead-acid battery." That is a cell-count figure and is not entered as a figure; it does not contradict S02, whose 48.5 to 50 V is the range a 1965 office held its 48-volt battery to, whereas 52.1 V is the page's nominal system voltage for 24 cells. Which the model uses is a choice for the item that builds the battery; "48 V" is the nominal for both.
- caveat:   Describes telephone plant in general, not 1965 specifically.

## S02 — central-office 48 V battery: operating limits
- figure:   48-volt battery held between 48.5 and 50 V; 24-volt battery 24 to 26 V; 38-volt battery 36 to 38 V.
- source:   Bell System Practices, Plant Series, Section 040-011-712, "Pulse Repeating Relays TA1 through TF2 ... Pulsing Requirements", Issue 1, July 1965, para 1.05 (AT&T Co Standard)
- url:      https://telephonecollectors.info/index.php/browse/bsps-bell-system/by-division-number/apparatus-tools/022-049-divisions-apparatus/040-division-relays/6319-040-011-712-i1r/file
- quote:    "The percent break values specified in this section are based on office battery limits of 24 to 26 volts for 24-volt battery, 36 to 38 volts for 38-volt battery and 48.5 to 50 volts for 48-volt battery."
- status:   sourced
- note:     Primary, full text. A 1965 document, so it fits the era. It gives the range the office battery is held to, not the voltage on a subscriber's pair after the line relay and loop drop.
- also:     *Notes on Distance Dialing* (S09's source) para 3.42 gives the sign for trunk signalling: "send ground or battery (-48 volts) signals to, the signaling circuit."

## S03 — resistance-design loop limit
- figure:   1300 ohms total loop resistance (the two wires of the pair, looped) for an unaided loop. 26 AWG alone reaches about 15,000 ft, 24 AWG about 24,000 ft, 22 AWG about 38,000 ft. Loading (88 mH every 6000 ft) on loops over 18,000 ft; bridged tap not over 5000 ft.
- source:   N. G. Long, "The Loop Plant: Part I -- Overview", glossary entry RESISTANCE DESIGN, *Bell System Technical Journal* 57(4), April 1978, p. 802
- url:      https://www.worldradiohistory.com/Archive-Bell-System-Technical-Journal/70s/Bell-System-Technical-Journal-1978-4.pdf
- quote:    "The standard resistance limit is 1300 ohms total (combined or looped resistance of the two pair wires)."
- status:   sourced
- note:     Primary, full text. The same issue (Hawley and Stiefel, "Voice Frequency Electronics for Loop Applications", p. 1088) says "unaided loop telephone service becomes marginal when loop cable resistance exceeds about 1300 ohms. To some extent this threshold is a function of switching system type."
- caveat:   1978, thirteen years after the era. The 1300-ohm figure is described there as the standing standard, but no 1965 document stating it was found. The same article proposes raising the standard to 1500 ohms for ESS offices, so the number has moved before. For a step-by-step office the line-circuit threshold (S07) is lower and is what bites.

## S04 — range-extended loop, maximum standard
- figure:   2800 ohms is the maximum standard range-extended loop.
- source:   Hawley and Stiefel, *BSTJ* 57(4), April 1978, p. 1088
- url:      https://www.worldradiohistory.com/Archive-Bell-System-Technical-Journal/70s/Bell-System-Technical-Journal-1978-4.pdf
- quote:    "Loops of 2800 ohms resistance, the maximum standard range -extended loop, draw a minimum of about 10 mA."
- status:   sourced
- note:     Primary, full text.
- caveat:   1978. Not needed before v0.2.

## S05 — copper resistivity (IACS)
- figure:   0.15328 ohm-gram/m^2 at 20 C; equivalently 0.017241 ohm-mm^2/m = 1.7241 microhm-cm at 20 C. (Conductivity 58 MS/m, "100% IACS", is derived by us as 1/1.7241e-8 ohm-m, not quoted from the Handbook, which gives 1.7241 = 100/58 in microhm-cm.) Temperature coefficient of resistance of the standard 0.00393 per C at 20 C.
- source:   *Copper Wire Tables*, National Bureau of Standards Handbook 100, issued February 21, 1966, section 1 (status of the International Annealed Copper Standard)
- url:      https://nvlpubs.nist.gov/nistpubs/Legacy/hb/nbshandbook100.pdf
- quote:    "The International Annealed Copper Standard, in various units of mass and volume resistivity, is: 0.153 28 ohm-gram/meter2 at 20 °C, ... 0.017 241 ohm-mm2/meter at 20 °C, 1.7241 microhm-cm at 20 °C, ..."
- status:   sourced
- note:     Primary, full text. Published within a year of the era. Also states "1.7241 microhm-cm" as 100/58 and the temperature coefficient "for this particular resistivity, is α20 =0.003 93 per °C".
- caveat:   This is the standard for *annealed* copper. Cable conductors were drawn copper; the model should say which it uses.

## S06 — AWG diameter definition, and cable-conductor resistance
- figure:   Diameters follow a geometric progression: No. 0000 is 0.4600 in, No. 36 is 0.0050 in, with 38 sizes between; d(n) = 0.005 in x 92^((36-n)/39), ratio between sizes 92^(1/39) = 1.122 932. Solid annealed copper at 20 C, ohms per 1000 ft: 22 AWG 16.2 (diameter 25.3 mil); 24 AWG 25.7 (20.1 mil); 26 AWG 41.0 (15.9 mil).
- source:   NBS Handbook 100 (1966), section 2.2 and Table 5, "Wire table, standard annealed copper, American Wire Gage, English units, values at 20 °C"
- url:      https://nvlpubs.nist.gov/nistpubs/Legacy/hb/nbshandbook100.pdf
- quote:    "Thus, the diameter of No. 0000 is defined as 0.4600 inch and of No. 36 as 0.0050 inch. There are 38 sizes between ..."
- status:   sourced
- note:     Primary, full text. The closed formula is the Handbook's geometric progression written out, and agrees with the formula at https://en.wikipedia.org/wiki/American_wire_gauge (secondary, fetched: "d n = 0.005 inch × 92(36−n)/39"). The Handbook says the gauge was revised in 1961 as ASTM B258-61. Table 5 is the source of the three resistance figures.
- caveat:   Loop resistance is twice the length times this value. Temperature moves it by 0.393% per C (S05).

## S07 — line-relay (call origination) detection current
- figure:   Office line circuits need a minimum of 10 to 16 mA to detect a call origination; some older small step-by-step offices with high-resistance line circuits provide only 7 mA.
- source:   Hawley and Stiefel, *BSTJ* 57(4), April 1978, p. 1088, section IV "Range Extension Functions", ORIGINATION
- url:      https://www.worldradiohistory.com/Archive-Bell-System-Technical-Journal/70s/Bell-System-Technical-Journal-1978-4.pdf
- quote:    "Office line circuits require a minimum of 10 to 16 mA to detect a call origination. ... Some older small step-by-step offices with high -resistance line circuits provide only 7 mA."
- status:   sourced
- note:     Primary, full text. This is a detection *threshold range across office types*, not the operate current of any one relay (see S11).
- caveat:   1978. "Older small step-by-step offices" is the closest thing to our case; 7 mA is far below the 20 mA the set wants (S08).

## S08 — loop current the telephone set is designed for
- figure:   The set performs acceptably over a 20 to 80 mA range of loop currents; below 20 mA the set's transmit gain suffers.
- source:   Hawley and Stiefel, *BSTJ* 57(4), April 1978, p. 1088, TELEPHONE SET CURRENT
- url:      https://www.worldradiohistory.com/Archive-Bell-System-Technical-Journal/70s/Bell-System-Technical-Journal-1978-4.pdf
- quote:    "The telephone set is designed to perform acceptably over a 20- to 80-mA range of loop currents."
- status:   sourced
- note:     Primary, full text.
- caveat:   1978, and not specific to the 500 set. Do not use as a 1965 500-set specification.

## S09 — dial pulse speed and percent break
- figure:   Later-type (present standard, 1956) customer dial: 9.5 to 10.5 pulses per second, 60 to 64 percent break. Earlier-type customer dial: 8.5 to 10.5 pps, 59.5 to 67.5 percent break. Nominal intertoll pulsing speed 9 to 11 pps; optimum toll pulsing speed 10 pps. Switchboard dials nominal 64 percent break.
- source:   *Notes on Distance Dialing*, American Telephone and Telegraph Company, Department of Operation and Engineering, September 1956, Section IV, Table 1 and paras 3.21 to 3.24
- url:      https://explodingthephone.com/hoppdocs/nodd1956.pdf
- quote:    "The Bell System readjust requirements for pulsing speed in pulses per second (pps), and per cent break of typical dial pulse generators are shown in Table 1." The table row reads "Later Type Customer Dial (Present Standard)   9.5-10.5   60-64".
- status:   sourced
- note:     Primary, full text (OCR). The table gives *readjust* limits: the tolerance to which service personnel set a dial, not the tolerance of the switching equipment. The 1956 edition; the 1961 and 1968 editions exist and were not read.
- also:     "Optimum toll dial pulsing speed may be considered as 10 pulses per second." (para 3.24.) Weaver and Newell (S20) call "the ideal 60-millisecond dial pulse", which is 60 percent of a 100 ms period.
- also:     Switching-equipment tolerance, https://en.wikipedia.org/wiki/Pulse_dialing (secondary): "the tolerance of the switching equipment was generally between 8 and 11 PPS."

## S10 — interdigit pause
- figure:   Minimum interdigital time 0.600 s when pulsing into step-by-step selectors or the equivalent; 0.300 s into crossbar or panel senders or registers. Timing accuracy of 5 per cent is satisfactory.
- source:   *Notes on Distance Dialing*, AT&T, September 1956, Section IV, paras 3.26 to 3.28
- url:      https://explodingthephone.com/hoppdocs/nodd1956.pdf
- quote:    "0.600 second when pulsing into step-by-step selectors or the equivalent." (para 3.27(2)); para 3.26 defines it: "The interdigital time is the interval from the end of the last on-hook pulse of one digit train to the beginning of the first on-hook pulse of the next digit train."
- status:   sourced
- note:     Primary, full text (OCR). These are requirements on a *sender* pulsing into a selector, not the interval a person's finger produces on a dial. Para 3.28 says three functions must be completed in a step-by-step office during the interval, and para 3.30 requires a stop-dial signal "0.065 second before the termination" of it. The first version of this entry wrongly said unsourced; the figure is in the same document as S09.
- caveat:   1956.

## S11 — line relay: operate and release current, operate and release time
- figure:   not found
- source:   none
- url:      none
- quote:    none
- status:   unsourced
- tried:    BSP 040-011-712 (1965) tests *toll-trunk* pulse-repeating relays at 6 to 12 pps; it does not specify a subscriber line relay. No operate current, release current or time for a step-by-step line relay appears in any fetched document. A British GPO Strowger page (dfrtelecoms.org.uk) was fetched but is a different relay set and is not a source for this office. The best available is the office-level threshold in S07.

## S12 — ringing voltage and frequency
- figure:   Ringing in North America about 90 V AC at 20 Hz, superimposed on the -48 V DC already on the line.
- source:   "Ringtone" (ring signal section), Wikipedia (secondary)
- url:      https://en.wikipedia.org/wiki/Ringtone
- quote:    "The ringing signal in North America is normally specified at ca. 90 volts AC with a frequency of 20 hertz."
- status:   sourced
- note:     Secondary. The frequency is corroborated by two primary texts. BSTJ April 1978: "signaling and 20 -Hz ringing currents from the DLL circuit to pass through to the loop". *Lenkurt Demodulator*, June 1968: "only one ringing frequency (usually 20 Hz) is needed to provide full-selective service to two customers over the same loop", and for superimposed ringing, "two sets of dc potentials of opposite polarities (+ 38 to + 48 Vdc) are applied to the tip and ring conductors for station selection" (https://worldradiohistory.com/Archive-Company-Publications/Lenkurt-Demodulator/60s/Lenkurt-Demodulator-1968-06.pdf; a carrier-equipment maker's trade publication).
- also:     A different quantity, not a corroboration of 90 V: *Notes on Distance Dialing* (1956) para 3.111, the ringing-start signal sent between offices, is "105V AC ringing current applied on a loop basis for a minimum of 0.35 second" (the "20 cycle" signal; the alternative SX signal is +130 V simplex for a minimum of 0.1 second). It is an interoffice signal, not the ringing a subscriber's bell receives.
- caveat:   The 90 V figure has no primary source here. Its tolerance, whether it is rms, and the 1965 value (which may differ with the generator type) are unsourced.

## S13 — ringing cadence: 2 seconds on, 4 seconds off
- figure:   2 s ringing, 4 s silent (a 6 s cycle).
- source:   "Ringtone", Wikipedia (secondary)
- url:      https://en.wikipedia.org/wiki/Ringtone
- quote:    "two seconds of ringing followed by four seconds of silence."
- status:   sourced
- note:     Secondary. Partly corroborated by *Notes on Distance Dialing* (1956), a test-line requirement: "Provide interrupted audible ringing tone during one 2-second ringing interval." The same list says "Test for tripping machine ringing during 3-second silent interval", which does not agree with 4 s. See S14.
- caveat:   The cadence is a property of the ringing interrupter and differs by office.

## S14 — ringing cadence: a conflicting primary
- figure:   Standard interrupter cycle for single-party service 6 s: 1.2 s of ring followed by 4.8 s of silence.
- source:   "Multiparty service through ringing", *Lenkurt Demodulator*, June 1968
- url:      https://worldradiohistory.com/Archive-Company-Publications/Lenkurt-Demodulator/60s/Lenkurt-Demodulator-1968-06.pdf
- quote:    "The standard interrupter ringing cycle for single-party service is 6 seconds — a 1.2-second ring followed by a 4.8-second period of silence."
- status:   sourced
- note:     Primary text, but a trade publication of a carrier-equipment maker, not an AT&T standard. It describes the interrupter as five groups each connected for 1.2 s, which is self-consistent. **Surprising:** it disagrees with S13 (2 s on, 4 s off), though both make a 6 s cycle. No AT&T document found settles it. The owner or the transmission department must pick one and record the choice in the item that makes it. The 2 s / 4 s form matches the 2 s interval in *Notes on Distance Dialing*.
- caveat:   1968.

## S15 — Precise Tone Plan: tones and levels
- figure:   Four frequencies, 350, 440, 480 and 620 Hz. Dial tone 350 + 440 Hz, continuous, -13 dBm. Audible ringing 440 + 480 Hz, -19 dBm, 2 s on / 4 s off. Busy (low) tone 480 + 620 Hz, -24 dBm, 0.5 s on / 0.5 s off. Reorder (fast busy): the busy tone at 0.25 s on / 0.25 s off.
- source:   "Precise tone plan", Wikipedia (secondary)
- url:      https://en.wikipedia.org/wiki/Precise_tone_plan
- quote:    "Dial tone is a continuous tone of the addition of the frequencies 350 and 440 Hz at a level of −13 dBm." and "Low tone, also busy tone, is defined as having frequency components of 480 and 620 Hz at a level of −24 dBm and a cadence of one half second ON and one half second OFF."
- status:   sourced
- note:     Secondary; the Bell System Practice that defines the plan was not found. These are **not** the tones of a 1965 step-by-step office; see S16.
- caveat:   The page gives no reference point for the levels.

## S16 — when the Precise Tone Plan came into service
- figure:   Standardization began with the first electronic switching system, a Western Electric 1ESS at Succasunna, NJ, in 1965. Before it, parts of the Bell System used "various similar signal frequencies and levels, without standardization".
- source:   "Precise tone plan", Wikipedia (secondary)
- url:      https://en.wikipedia.org/wiki/Precise_tone_plan
- quote:    "The standardization process began with the installation of the first electronic switching system, a Western Electric 1ESS at Succasunna, NJ in 1965."
- status:   sourced
- note:     Secondary. **This answers the question the era spike left open** ("Precise Tone Plan if in service then, establish that"). In a step-by-step office about 1965 it was not: the plan begins with the first ESS that year, and a step-by-step office made its tones with electromechanical tone alternators and interrupters (S17). The year the plan reached step-by-step offices was not found and is unsourced.

## S17 — the tones before the Precise Tone Plan (1956)
- figure:   Dial tone and low tone: 600 Hz modulated by 120 Hz from a tone alternator, or by 133 Hz from an interrupter. Interrupted low tone serves for line busy (60 interruptions per minute, i.e. 0.5 s on / 0.5 s off), reorder (120 ipm) and no-circuit (30 ipm). High tone nominally 500 Hz from a tone alternator or 400 Hz from an interrupter. Audible ringing: 420 Hz modulated with 40 Hz. Test tone 1000 Hz. Audible-ringing level limit about 65 dBa.
- source:   *Notes on Distance Dialing* (AT&T, September 1956), Section IV, paras 3.101, 3.102, 3.107, 3.109 and para 3.03(3)
- url:      https://explodingthephone.com/hoppdocs/nodd1956.pdf
- quote:    "These are 600 cps modulated by 120 cps when supplied by a tone alternator or by 133 cps when supplied by an interrupter." and "420 cps modulated with 40 cps is a typical audible ringing signal." and "Frequency of Repetition: Examples are: No Circuit (NC), 30 IPM; Line Busy, 60 IPM; and Reorder, 120 IPM."
- status:   sourced
- note:     Primary, full text (OCR; "cps" is cycles per second). The nearest document to the era found, nine years early. Tone *levels* and dial-tone start delay are not in this text. Whether a 1965 step-by-step office still made the 1956 tones is not shown.
- caveat:   1956.

## S18 — voice-band edges
- figure:   300 to 3400 Hz, sampled at 8000 Hz.
- source:   ITU-T Recommendation G.711, "Pulse code modulation (PCM) of voice frequencies", approved 25 November 1988 (first issued December 1972), as summarised by "G.711", Wikipedia (secondary)
- url:      https://en.wikipedia.org/wiki/G.711
- quote:    "the frequency band of 300–3400 Hz and samples them at the rate of 8000 Hz" (as extracted by the fetch tool).
- status:   sourced
- note:     Secondary. The ITU page (https://www.itu.int/rec/T-REC-G.711-198811-I/en) was fetched; it gave the title and approval date, not the technical text, so the band edges are not quoted from the Recommendation itself.
- caveat:   G.711 is 1972, after the era. No 1965 document defining the voice-band edges was found.

## S19 — mu-law definition
- figure:   F(x) = sgn(x) ln(1 + mu|x|) / ln(1 + mu) for -1 <= x <= 1, mu = 255 in North America. G.711 encodes 14-bit linear samples to 8 bits, 64 kbit/s at 8 kHz.
- source:   "μ-law algorithm", Wikipedia (secondary); G.711 per S18
- url:      https://en.wikipedia.org/wiki/%CE%9C-law_algorithm
- quote:    "F(x)=sgn(x) ln(1+μ|x|)/ln(1+μ), −1≤x≤1" (as extracted by the fetch tool).
- status:   sourced
- note:     Secondary; G.711's own tables were not fetched. The continuous formula is a definition, but the codec G.711 specifies is the *segmented* 8-bit approximation, and a check of the codec against G.711's segment table is **unsourced** here.
- caveat:   A 1965 step-by-step office has no PCM; mu-law enters at v0.5 and is the only part of the plan that is not the office's own.

## S20 — SF signalling: level, timing and guard
- figure:   Idle tone -20 dBm at zero transmission level; a boost at the start of each signal of 12 dB for the 2600-cycle system (14 dB for 1600-cycle); receiver sensitivity -28 dBm (8 dB overall margin); guard-to-signal ratio 6 to 10 dB; minimum signal duration to operate 50 ms; the shortest signal element may be as low as 30 ms; maximum permissible transmission time of a signal between trunk terminals about 175 ms; receiver bandwidth 60 to 75 cycles at the 3 dB points at dialing power (about -6 dBm at zero level).
- source:   A. Weaver and N. A. Newell, "In-Band Single-Frequency Signaling", *Bell System Technical Journal*, November 1954 (manuscript received June 7, 1954), pp. 1309-1330 (volume and page range per Wikipedia; page headers in the scan read 1312 to 1317), and Table III
- url:      https://explodingthephone.com/hoppdocs/weaver1954.pdf
- quote:    "A value of -20 dbm referred to zero transmission level for the steady idle tone is satisfactory for this purpose." and "A guard ratio in the range of 6 to 10 db has been found to be practicable." Table III: "Minimum duration signal for operate ... 50 ms".
- status:   sourced
- note:     Primary, full text (OCR; "db", "dbm" are dB, dBm). It treats the 1600-cycle and 2600-cycle systems together; the figures above say where they differ. "Guard action" is described as the principal protection against operation on speech: nearly all the voice-band frequencies other than a narrow band around the signalling frequency generate a voltage that opposes the signal. The paper's graphs (Figs. 2 and 4) were not read.
- caveat:   1954, eleven years before the era; the 2600-cycle unit was the later system (S21) and its level and timing may have been revised. The SF unit's own tone-off and tone-on recognition times are not in the text as read and are unsourced.

## S21 — SF signalling frequency and supervision
- figure:   2600 Hz on 4-wire trunks; 2600 and 2400 Hz on 2-wire trunks. Tone is on when idle (on-hook) and off when the trunk is off-hook; the SF unit connects to the trunk circuit by E and M leads. The 2600-cycle system cannot be used on narrow-band facilities (EB channel banks, H-172 loaded lines).
- source:   *Notes on Distance Dialing* (AT&T, September 1956), Section IV, paras 3.67 to 3.70
- url:      https://explodingthephone.com/hoppdocs/nodd1956.pdf
- quote:    "The later, 2600-cycle, SF system employs 2600 cycles for 4-wire trunks and 2600 and 2400 cycles for 2-wire trunks."
- status:   sourced
- note:     Primary, full text (OCR). The table at 3.70: on-hook, tone on, sending M ground, receiving E open; off-hook, tone off, sending M battery, receiving E ground. A 1956 document calls the 2600-cycle system "the later" one, so it was in service by 1956, consistent with the era. SF systems "operate on a pulse correcting basis with a median 58 per cent break at 10 pulses per second" (after Table 2).
- also:     https://en.wikipedia.org/wiki/Single-frequency_signaling (secondary): "The SF tone is present in the on-hook or idle state and absent during the seized state."

## S22 — the 500-type ringer
- figure:   The ringer is the C2A: a single coil with two windings (so one ringer serves two-party service), a loudness control over about 15 dB, and a higher impedance at ringing frequency than the B1A that "permits the use of five ringers, either bridged or between each wire and ground, instead of four as in the past."
- source:   Bell System Practices, Section AB22.066.1, "500 Series Combined Telephone Sets: General Description", Issue 1, May 1952, para 4.09 and Fig. 13
- url:      https://www.telephonecollectors.info/index.php/browse/document-repository/bsp-bell-system-practices-by-doc/bsp-categories-by-early-letter-code-by-doc/14877-ab22-066-1-i1-may52-500-set-description/file
- quote:    "The higher impedance at ringing frequency permits the use of five ringers , either bridged or between each wire and ground , instead of four as in the past."
- status:   sourced
- note:     Primary, full text (OCR), 1952. Sourced for the ringer's structure and how many may share a line, and for nothing numeric about its electrical values; those are S30. The *Lenkurt Demodulator* (1968) adds that in series with the ringer is a capacitor "to prevent flow of direct current through the ringer coils", which "coupled with ringer inductance resonates at about the ringing frequency".

## S23 — office impedance (for the cable and the term set)
- figure:   Local offices 900 ohms; manual toll offices 600 ohms; step-by-step intertoll 600 ohms.
- source:   *Notes on Distance Dialing* (1956), para 4.49 and the table following it
- url:      https://explodingthephone.com/hoppdocs/nodd1956.pdf
- quote:    "Traditionally, the toll office impedance has been 600 ohms and the local office impedance has been 900 ohms."
- status:   sourced
- note:     Primary, full text (OCR).
- caveat:   1956.

## S24 — the Strowger patent
- figure:   US Patent 447,918, "Automatic Telephone-Exchange", Almon B. Strowger; filed March 12, 1889; granted March 10, 1891.
- source:   Google Patents record for US447918A
- url:      https://patents.google.com/patent/US447918
- quote:    "My invention relates to an improvement in automatic, telephonic, telegraphic, and other electrical exchanges."
- status:   sourced
- note:     The number is also confirmed at https://www.invent.org/inductees/almon-brown-strowger ("U.S. Patent No. 447,918") (secondary). The dates are from the Google Patents page as extracted by the fetch tool. The drawings and claims were not read.

## S25 — La Porte, 1892
- figure:   The first commercial Strowger exchange opened on November 3, 1892 in La Porte, Indiana, with about 75 subscribers and capacity for 99.
- source:   "Strowger switch" and "Almon Brown Strowger", Wikipedia (secondary)
- url:      https://en.wikipedia.org/wiki/Strowger_switch
- quote:    "The company installed and opened the first commercial exchange in his then-home town of La Porte, Indiana on November 3, 1892." and, from the biography page (https://en.wikipedia.org/wiki/Almon_Strowger), "with about 75 subscribers and capacity for 99."
- status:   sourced
- note:     Secondary, with partial support: https://www.invent.org/inductees/almon-brown-strowger ("The first automatic telephone exchange was installed in La Porte, Indiana in 1892.") and https://www.kansashistory.gov/p/almon-strowger/16911 ("the first working system into La Porte, Indiana, in 1892"). The Kansas page also says "In 1892 Strowger patented his automatic telephone exchange", which conflicts with S24 (granted 1891); do not repeat it. The 75 subscribers appears only in the Wikipedia pages.
- caveat:   The phrase "his then-home town" is not corroborated and the project does not use it.

## S26 — what the first Strowger system's "dial" was
- figure:   The 1892 system was not dialled. Four keys, one each for thousands, hundreds, tens and units, were added near the telephone. The finger-wheel dial was patented in 1896.
- source:   *Lenkurt Demodulator*, June 1968, "Strowger Switch"; and "Strowger switch", Wikipedia (secondary)
- url:      https://worldradiohistory.com/Archive-Company-Publications/Lenkurt-Demodulator/60s/Lenkurt-Demodulator-1968-06.pdf
- quote:    "At first, pushbuttons were used for “dialing”, but were followed by the rotary finger-wheel dial similar to those used today." and (Wikipedia, https://en.wikipedia.org/wiki/Strowger_switch) "In 1896 the company patented a finger-wheel dial as an improvement to the existing four-key design."
- status:   sourced
- note:     The Lenkurt text is primary but a trade-press history, and "pushbuttons" is looser than Wikipedia's "four keys"; they do not contradict. The claim in the item that 1892 had "no dial tone" has no source in this ledger and is not asserted here.
- caveat:   Strowger's motive (an undertaker, an operator diverting calls to a competitor) is told in the same Lenkurt text ("he was losing valuable business through the partiality of an operator") and at the Kansas page above; it is a story told about him, not a figure.

## S27 — the selector: ten levels, ten contacts
- figure:   A contact arm moves up to one of ten rows, then rotates to one of ten contacts in that row: 100 choices.
- source:   "Strowger switch", Wikipedia (secondary)
- url:      https://en.wikipedia.org/wiki/Strowger_switch
- quote:    "A contact arm is moved up to select one of ten rows of contacts, and then rotated clockwise to select one of ten contacts in that row, a total of 100 choices."
- status:   sourced
- note:     Secondary. It is a description, and the derived count (10 x 10 = 100) is a better judge of the model than the page.

## S28 — step-by-step was still the most common switch about 1965
- figure:   the claim in item 0009 that step-by-step "was still the most common switch in service"
- source:   none
- url:      none
- quote:    none
- status:   unsourced
- tried:    Wikipedia "Strowger switch" and "Step-by-step switch" (fetched; neither gives a Bell System share or a 1965 count; one says only that Strowger systems "were widespread through most of the 20th century, being gradually relegated to small community service"). *Notes on Distance Dialing* (1956) names step-by-step intertoll selectors but gives no count. The claim is plausible; the README should not state it as fact without a source.

## S29 — ring trip, and dial-tone delay
- figure:   not found
- source:   none
- url:      none
- quote:    none
- status:   unsourced
- tried:    A ring-trip threshold, how fast ringing is removed when the called party answers, and the dial-tone delay allowed in a step-by-step office are not given in any fetched document. BSTJ April 1978 says only that "Loop currents when a called party answers during ringing are greater than those during origination; however, office circuit thresholds are also higher".

## S30 — the 500 set's DC resistance, ringer resistance and capacitor, and network values
- figure:   not found
- source:   none
- url:      none
- quote:    none
- status:   unsourced
- tried:    BSP AB22.066.1 (1952) describes the set and gives graphs, with no electrical values for these; BSP 502-510-100 (1967, identification, installation and maintenance) was fetched and gives none; Wikipedia "Model 500 telephone" and prc68.com's 500-set page were fetched and give none. Values commonly quoted for a ringer capacitor and ringer impedance came from search summaries only and are not entered. Until this is sourced, a check of the set's loop resistance or the bridge's balance has nothing external to be judged against, and the README must say so next to the result.

---

## Totals

30 entries.

- Sourced: 26 (S01 to S10, S12 to S27). Sixteen rest on a primary document read in full text (S02 to S10, S14, S17, S20 to S23, and S26 in part); ten rest on a secondary source alone (S01, S12, S13, S15, S16, S18, S19, S24, S25, S27), though S12 and S13 are partly corroborated by a primary.
- Unsourced: 4 (S11 line-relay operate and release; S28 step-by-step prevalence; S29 ring trip and dial-tone delay; S30 the 500 set's DC resistance and ringer values).
- Not the era's year: S03, S04, S07, S08 (1978); S09, S10, S17, S21, S23 (1956); S20 (1954); S14 (1968); S18, S19 (1972 or later). Within a year of 1965: S02 (1965), S05 and S06 (1966).
- Conflicts between sources: S13 against S14 (ringing cadence), and S25 against the Kansas page on the patent year.
