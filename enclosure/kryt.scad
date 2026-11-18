// =====================================================================
//  Rest Pause Tracker - kryt pro 3D tisk (verze 1)
//  Dva dily: VANA (dno + steny) a VIKO (celni deska s displejem).
//  Viko se k vane prisroubuje 4 srouby M2 x 10 do sloupku v rozich.
//  Na zadni strane (dno vany) 2 magnety M3 pro prichyceni na stojan / ocel.
//
//  Jak pouzit: otevri v OpenSCAD, nahore nastav `cast`, stiskni F6 (render)
//  a pak F7 (export STL). Pro kazdy dil zvlast.
//
//  Souradnice (pohled zepredu na displej):
//    x = doprava, y = nahoru, z = smerem k tobe (od dna vany k viku).
//    Pocatek = vnitrni levy dolni roh vany na vnitrni strane dna.
// =====================================================================

cast = "sestava";   // "vana" | "viko" | "hmatnik" | "sestava" | "bez_vika" | "rez"

$fn = 48;

// ---------- Krabicka ----------
stena   = 2.0;      // tloustka bocnich sten
dno     = 2.0;      // tloustka dna vany
viko_t  = 2.0;      // tloustka vika
vnitr_w = 44;       // vnitrni sirka  (x)
vnitr_h = 68;       // vnitrni vyska  (y)
vnitr_d = 32;       // vnitrni hloubka (z) = od dna po vnitrni stranu vika
roh_r   = 3;        // zaobleni vnejsich rohu
vule    = 0.25;     // vule pro tisk (otvory o tolik vetsi)

// ---------- Sroubky vika (rohove sloupky) ----------
sloupek_d    = 6.5;
sloupek_od   = 3.5;   // stred sloupku od vnitrnich sten
m2_pilot     = 1.7;   // otvor pro zavrtani M2 do plastu
m2_volny     = 2.4;   // otvor, kterym M2 projde volne
m2_hlava_d   = 4.2;   // zahloubeni pro hlavu sroubu ve viku
m2_hlava_h   = 1.2;

// ---------- Displej (vykres LaskaKit OLED_1_3INCH, 30.01.2025) ----------
disp_w = 35.5;  disp_h = 33.5;  disp_pcb = 1.57;
disp_x = (vnitr_w - disp_w) / 2;     // poloha desky displeje ve vnitru
disp_y = 27;
disp_diry   = [[3, 2.5], [33.5, 2.5], [3, 31], [33.5, 31]];  // vuci levemu dolnimu rohu desky
sklo_pos    = [0.45, 5.26];  sklo_vel = [34.6, 23];
sklo_vyska  = 3.29 - disp_pcb;           // o kolik sklo vycniva pred desku (1.72)
pix_pos     = [3.0, 11.46];  pix_vel  = [29.5, 14.8];   // svitici plocha
okno_okraj  = 0.75;                      // okno o tolik vetsi nez pixely kazdym smerem
disp_sloupek_h = sklo_vyska + 0.1;       // sklo konci 0.1 mm pred vikem
disp_sloupek_d = 5;
disp_konektor  = 2.9;                    // konektor uSup za deskou

// ---------- Pasek s tlacitky (kus univerzalniho spoje 16 x 5 direk) ----------
pas_w = 40.6;  pas_h = 12.7;  pas_t = 1.6;
pas_x = (vnitr_w - pas_w) / 2;
pas_y = 8.5;
tl_vyska    = 5.0;    // tlacitko 6x6x5: od desky po vrsek hmatniku
hm_lem_t    = 1.5;    // lem hmatniku (zarazka za vikem)
hm_lem_d    = 10;
hm_d        = 8;      // prumer hmatniku, ktery projde vikem
hm_vycniva  = 1.5;    // o kolik hmatnik vycniva z vika
pas_od_vika = hm_lem_t + 0.3 + tl_vyska;   // mezera vnitrni strana vika -> predni strana pasku (6.8)
pas_sroub_x = [pas_x + 2, pas_x + pas_w - 2];  // otvory Ø2.2 vyvrtat 2 mm od koncu pasku
tl_y   = pas_y + pas_h / 2;
tl_A_x = vnitr_w / 2 - 10;
tl_B_x = vnitr_w / 2 + 10;
led_x  = vnitr_w / 2;
led_d  = 5.0;

// ---------- Hlavni deska (univerzalni spoj 16 x 12 direk) + XIAO ----------
deska_w = 41;  deska_h = 31;  deska_t = 1.6;
deska_x = (vnitr_w - deska_w) / 2;
deska_y = 23;
deska_od_vika = 23;                          // vnitrni strana vika -> predni strana desky
deska_z = vnitr_d - deska_od_vika - deska_t; // spodni strana desky (= vyska lyzin)
lyzina_w = 3;                                // lyziny podel leve a prave steny, deska na nich lezi

lista_h  = 11.0;  // od desky po spodek XIAO: dutinkova lista 8.5 + plast kolicku XIAO 2.5 (zmereno 2026-11-15)
xiao_w = 21;  xiao_h = 17.8;  xiao_t = 1.2;
usb_w = 9;   usb_t = 3.2;  usb_presah = 1.0;
xiao_x = deska_x + deska_w - xiao_w;          // USB-C na pravem okraji desky
xiao_y = deska_y + (deska_h - xiao_h) / 2;
xiao_z = deska_z + deska_t + lista_h;         // spodek plosky XIAO
usb_z  = xiao_z + xiao_t + usb_t / 2;          // stred konektoru USB-C
usb_otvor = [12.5, 7.5];                      // otvor v prave stene (y, z)

// ---------- Baterie 503035 ----------
bat = [30, 35, 5];
bat_x = (vnitr_w - bat[0]) / 2;
bat_y = deska_y + deska_h / 2 - bat[1] / 2;

// ---------- Posuvny vypinac (leva stena, paka se posouva ve smeru z) ----------
vyp_y      = 57;          // stred vypinace
vyp_z      = 19;
vyp_drazka = [6.2, 3.2];  // otvor pro paku (z, y)
vyp_rozt   = 15;          // roztec der Ø2.2 v prirube
vyp_priruba = [19.6, 5.5];
vyp_telo    = [11.5, 5.5, 5.0];

// ---------- Magnety na zadni strane (dno vany) ----------
// Magnet v pouzdre Ø10 x 3 mm se zavitem M3 dlouhym 4 mm, matice M3 (DIN 934: 5.5 mm na plosky, 2.4 mm vysoka).
// Magnet sedi v zahloubeni zvenku, zavit projde dnem, matice se utahne zevnitr.
mag_d       = 10;
mag_t       = 3;
mag_vycniva = 0.2;    // o kolik magnet vycniva ze dna (aby se dotykal oceli)
mag_mustek  = 1.0;    // plast mezi magnetem a matici
mag_zavit_d = 3.4;
mag_vystupek_d = 12;  // zesileni dna kolem matice (zevnitr)
mag_pozice  = [[vnitr_w / 2, 10], [vnitr_w / 2, 62.5]];   // pod paskem tlacitek a nad baterii
mag_zahl    = mag_t - mag_vycniva;                         // hloubka zahloubeni od vnejsiho povrchu
mag_vystupek_h = mag_zahl + mag_mustek - dno;              // o kolik zesileni vystupuje nad vnitrni dno

// =====================================================================
//  Pomocne tvary
// =====================================================================
module zaobleny_kvadr(w, h, t, r) {
    hull()
        for (x = [r, w - r], y = [r, h - r])
            translate([x, y, 0]) cylinder(r = r, h = t);
}

sloupky_xy = [[sloupek_od, sloupek_od],
              [vnitr_w - sloupek_od, sloupek_od],
              [sloupek_od, vnitr_h - sloupek_od],
              [vnitr_w - sloupek_od, vnitr_h - sloupek_od]];

// =====================================================================
//  VANA
// =====================================================================
module vana() {
    difference() {
        union() {
            // plast
            difference() {
                translate([-stena, -stena, -dno])
                    zaobleny_kvadr(vnitr_w + 2 * stena, vnitr_h + 2 * stena, dno + vnitr_d, roh_r);
                translate([0, 0, 0])
                    cube([vnitr_w, vnitr_h, vnitr_d + 1]);
            }
            // rohove sloupky (spojene se stenami)
            for (p = sloupky_xy) {
                translate([p[0], p[1], 0]) cylinder(d = sloupek_d, h = vnitr_d);
                // vypln mezi sloupkem a rohem
                translate([p[0] < vnitr_w / 2 ? 0 : p[0], p[1] < vnitr_h / 2 ? 0 : p[1], 0])
                    cube([sloupek_od, sloupek_od, vnitr_d]);
            }
            // lyziny pro hlavni desku
            for (x = [0, vnitr_w - lyzina_w])
                translate([x, deska_y, 0]) cube([lyzina_w, deska_h, deska_z]);
            // zesileni dna pod maticemi magnetu
            for (p = mag_pozice)
                translate([p[0], p[1], 0]) cylinder(d = mag_vystupek_d, h = mag_vystupek_h);
        }
        // magnety: zahloubeni zvenku + dira pro zavit
        for (p = mag_pozice) {
            translate([p[0], p[1], -dno - 0.01]) cylinder(d = mag_d + 2 * vule, h = mag_zahl + 0.01);
            translate([p[0], p[1], -dno - 1]) cylinder(d = mag_zavit_d, h = dno + mag_vystupek_h + 2);
        }
        // otvory pro srouby vika
        for (p = sloupky_xy)
            translate([p[0], p[1], vnitr_d - 12]) cylinder(d = m2_pilot, h = 13);

        // USB-C v prave stene
        translate([vnitr_w - 1, xiao_y + xiao_h / 2, usb_z])
            rotate([0, 90, 0])
                hull() {
                    r = 2;
                    for (dy = [-usb_otvor[0] / 2 + r, usb_otvor[0] / 2 - r],
                         dz = [-usb_otvor[1] / 2 + r, usb_otvor[1] / 2 - r])
                        translate([dz, dy, 0]) cylinder(r = r, h = stena + 2);
                }

        // vypinac v leve stene: drazka pro paku + 2 diry pro M2
        translate([-stena - 1, vyp_y - (vyp_drazka[1] + vule) / 2, vyp_z - (vyp_drazka[0] + vule) / 2])
            cube([stena + 2, vyp_drazka[1] + vule, vyp_drazka[0] + vule]);
        for (dz = [-vyp_rozt / 2, vyp_rozt / 2])
            translate([-stena - 1, vyp_y, vyp_z + dz])
                rotate([0, 90, 0]) cylinder(d = m2_volny, h = stena + 2);
    }
}

// =====================================================================
//  VIKO (modelovano na svem miste v sestave, z od vnitrni strany vika)
// =====================================================================
module viko() {
    difference() {
        union() {
            translate([-stena, -stena, vnitr_d])
                zaobleny_kvadr(vnitr_w + 2 * stena, vnitr_h + 2 * stena, viko_t, roh_r);
            // sloupky pro displej
            for (d = disp_diry)
                translate([disp_x + d[0], disp_y + d[1], vnitr_d - disp_sloupek_h])
                    cylinder(d = disp_sloupek_d, h = disp_sloupek_h);
            // sloupky pro pasek s tlacitky
            for (x = pas_sroub_x)
                translate([x, tl_y, vnitr_d - pas_od_vika])
                    cylinder(d = 5, h = pas_od_vika);
        }
        // okno displeje (se zkosenim smerem ven, at je dobre videt ze strany)
        translate([disp_x + pix_pos[0] - okno_okraj, disp_y + pix_pos[1] - okno_okraj, vnitr_d - 0.01])
            hull() {
                cube([pix_vel[0] + 2 * okno_okraj, pix_vel[1] + 2 * okno_okraj, 0.01]);
                translate([-viko_t, -viko_t, viko_t])
                    cube([pix_vel[0] + 2 * okno_okraj + 2 * viko_t,
                          pix_vel[1] + 2 * okno_okraj + 2 * viko_t, 0.02]);
            }
        // pilotni diry do sloupku displeje (neprochazi celnim povrchem)
        for (d = disp_diry)
            translate([disp_x + d[0], disp_y + d[1], vnitr_d - disp_sloupek_h - 0.01])
                cylinder(d = m2_pilot, h = disp_sloupek_h + viko_t - 0.8);
        // pilotni diry do sloupku pasku
        for (x = pas_sroub_x)
            translate([x, tl_y, vnitr_d - pas_od_vika - 0.01])
                cylinder(d = m2_pilot, h = pas_od_vika + viko_t - 0.8);
        // diry pro hmatniky A a B
        for (x = [tl_A_x, tl_B_x])
            translate([x, tl_y, vnitr_d - 1]) cylinder(d = hm_d + 2 * vule, h = viko_t + 2);
        // dira pro LED
        translate([led_x, tl_y, vnitr_d - 1]) cylinder(d = led_d + 2 * vule, h = viko_t + 2);
        // srouby vika: dira + zahloubeni pro hlavu
        for (p = sloupky_xy) {
            translate([p[0], p[1], vnitr_d - 1]) cylinder(d = m2_volny, h = viko_t + 2);
            translate([p[0], p[1], vnitr_d + viko_t - m2_hlava_h]) cylinder(d = m2_hlava_d, h = m2_hlava_h + 1);
        }
        // popisky A / B (vyryte 0.6 mm)
        for (t = [[tl_A_x, "A"], [tl_B_x, "B"]])
            translate([t[0], tl_y - hm_lem_d / 2 - 3.5, vnitr_d + viko_t - 0.6])
                linear_extrude(1)
                    text(t[1], size = 4, halign = "center", valign = "center",
                         font = "Liberation Sans:style=Bold");
    }
}

// =====================================================================
//  HMATNIK (tisknout 2x, lemem dolu)
// =====================================================================
module hmatnik() {
    cylinder(d = hm_lem_d, h = hm_lem_t);
    cylinder(d = hm_d, h = hm_lem_t + viko_t + hm_vycniva);
}

// =====================================================================
//  Soucastky jen pro kontrolu v sestave (netisknou se)
// =====================================================================
module soucastky() {
    // displej: deska + sklo + konektor vzadu
    color("darkblue")
        translate([disp_x, disp_y, vnitr_d - disp_sloupek_h - disp_pcb]) cube([disp_w, disp_h, disp_pcb]);
    color("black")
        translate([disp_x + sklo_pos[0], disp_y + sklo_pos[1], vnitr_d - disp_sloupek_h])
            cube([sklo_vel[0], sklo_vel[1], sklo_vyska]);
    color("gray")
        translate([disp_x + 10, disp_y + 12, vnitr_d - disp_sloupek_h - disp_pcb - disp_konektor])
            cube([6, 4, disp_konektor]);
    // pasek s tlacitky + LED
    color("green")
        translate([pas_x, pas_y, vnitr_d - pas_od_vika - pas_t]) cube([pas_w, pas_h, pas_t]);
    for (x = [tl_A_x, tl_B_x])
        color("dimgray") translate([x - 3, tl_y - 3, vnitr_d - pas_od_vika]) cube([6, 6, tl_vyska]);
    color("red") translate([led_x, tl_y, vnitr_d - pas_od_vika]) cylinder(d = led_d, h = 8.6 - 1);
    // hlavni deska + lista + XIAO + USB-C
    color("green") translate([deska_x, deska_y, deska_z]) cube([deska_w, deska_h, deska_t]);
    for (dy = [0.5, xiao_h - 3])
        color("black") translate([xiao_x + 1, xiao_y + dy, deska_z + deska_t]) cube([18, 2.5, lista_h]);
    color("purple") translate([xiao_x, xiao_y, xiao_z]) cube([xiao_w, xiao_h, xiao_t]);
    color("silver")
        translate([xiao_x + xiao_w - 7.3 + usb_presah, xiao_y + (xiao_h - usb_w) / 2, xiao_z + xiao_t])
            cube([7.3, usb_w, usb_t]);
    // baterie
    color("orange") translate([bat_x, bat_y, 0]) cube(bat);
    // magnety + matice
    for (p = mag_pozice) {
        color("silver") translate([p[0], p[1], -dno - mag_vycniva]) cylinder(d = mag_d, h = mag_t);
        color("silver") translate([p[0], p[1], -dno - mag_vycniva + mag_t]) cylinder(d = 3, h = 4);
        color("gray") translate([p[0], p[1], mag_vystupek_h]) cylinder(d = 6.35, h = 2.4, $fn = 6);
    }
    // vypinac (priruba na vnitrni strane leve steny, telo smerem dovnitr)
    color("white")
        translate([0, vyp_y - vyp_priruba[1] / 2, vyp_z - vyp_priruba[0] / 2])
            cube([0.6, vyp_priruba[1], vyp_priruba[0]]);
    color("white")
        translate([0.6, vyp_y - vyp_telo[1] / 2, vyp_z - vyp_telo[0] / 2])
            cube([vyp_telo[2], vyp_telo[1], vyp_telo[0]]);
}

// =====================================================================
if (cast == "vana") {
    vana();
} else if (cast == "viko") {
    // otocit celnim povrchem na podlozku tiskarny
    translate([0, vnitr_h, vnitr_d + viko_t]) rotate([180, 0, 0]) viko();
} else if (cast == "hmatnik") {
    hmatnik();
    translate([hm_lem_d + 3, 0, 0]) hmatnik();
} else if (cast == "nic") {
    // nic nevykreslit (pro include z jinych souboru)
} else if (cast == "bez_vika") {
    // pohled dovnitr vany se soucastkami
    vana();
    soucastky();
} else if (cast == "rez") {
    // rez sestavou v polovine sirky (kontrola hloubek)
    intersection() {
        union() { vana(); viko(); soucastky(); }
        translate([-10, -10, -10]) cube([vnitr_w / 2 + 10, vnitr_h + 20, vnitr_d + 20]);
    }
} else {
    vana();
    color("lightblue", 0.5) viko();
    soucastky();
}
