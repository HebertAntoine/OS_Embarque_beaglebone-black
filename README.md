# ---------------------------------------------------------
#   INTRODUCTION
# ---------------------------------------------------------

Ce projet montre comment :

✔️ Accéder aux **GPIO** et **PWM** de la BeagleBone Black  
✔️ À partir d’un **module noyau Linux (.ko)**  
✔️ En utilisant un **cross-compilateur dans Docker**  
✔️ Et en **chargeant automatiquement les modules au boot**  

Deux modules sont générés :

- `gpio_blink.ko` → fait clignoter **4 GPIO** (1 Hz)
- `pwm_kernel.ko` → génère **4 PWM logiciels** (50 Hz / 50 %)

L’objectif demandé dans le cours est rempli :

> **« Très Bien : capable de cross-compiler un module noyau qui accède aux GPIO / PWM »**

Ce README contient :

- Les broches utilisées  
- Le pinout  
- Le tutoriel cross-compile complet  
- Les commandes pour charger les modules au démarrage  
- Les commandes pour mesurer le temps de boot  

---

# ---------------------------------------------------------
#   BROCHES UTILISÉES (GPIO + PWM)
# ---------------------------------------------------------

## 🟩 GPIO utilisés par le module `gpio_blink.ko`

| Fonction               | Broche BBB | Nom TI (bank + numéro) | GPIO Linux |
|-----------------------|------------|-------------------------|------------|
| GPIO Blink #1         | P8_10      | GPIO2_4                 | **68**     |
| GPIO Blink #2         | P8_8       | GPIO2_3                 | **67**     |
| GPIO Blink #3         | P8_9       | GPIO2_5                 | **69**     |
| GPIO Blink #4         | P8_26      | GPIO1_29                | **61**     |

🔎 Conversion :  
`GPIO(bank, number) = bank * 32 + number`

---

## 🟦 PWM utilisés par le module `pwm_kernel.ko`

| Fonction PWM           | Broche BBB | Périphérique interne | Chip / Index |
|------------------------|------------|-----------------------|--------------|
| PWM #1                 | P9_14      | ehrpwm1A              | pwmchip4/0   |
| PWM #2                 | P9_16      | ehrpwm1B              | pwmchip4/1   |
| PWM #3                 | P9_21      | ehrpwm0A              | pwmchip0/0   |
| PWM #4                 | P9_22      | ehrpwm0B              | pwmchip0/1   |

---

## 🖼️ PINOUT BEAGLEBONE BLACK (Référence)

![BeagleBone Black Pinout](https://raw.githubusercontent.com/mwelling/bbb-pinout/master/images/BBB_Pinout.png)

![alt text](BeagleBone_Black_Pinout.png.webp)

---

# ---------------------------------------------------------
#   1. RÉCUPÉRER LE NIVEAU DU NOYAU DE LA BBB   
# ---------------------------------------------------------

# Sur la BeagleBone
```bash 
uname -r
```
# Exemple : 4.19.94-ti-r42

# Vérifier la présence des headers
```bash 
ls -l /lib/modules/$(uname -r)/build
```
# (Sur Debian BBB, il n'existe pas → il faut les installer)

# Télécharger manuellement le .deb (depuis un PC avec internet)
# Fichier utilisé :
# linux-headers-4.19.94-ti-r42_1stretch_armhf.deb

# Copier ce fichier vers la BBB
```bash 
scp linux-headers-4.19.94-ti-r42_1stretch_armhf.deb debian@192.168.6.2:/home/debian/
```

# Sur la BeagleBone : installation des headers
```bash 
sudo dpkg -i linux-headers-4.19.94-ti-r42_1stretch_armhf.deb
```

# Vérifier que le kernel est maintenant accessible :
```bash 
ls -l /lib/modules/4.19.94-ti-r42/build
```
# Doit afficher un lien -> /usr/src/linux-headers-4.19.94-ti-r42

# ---------------------------------------------------------
#   2. COPIER LES HEADERS DE LA BBB VERS LE MAC (CROSS)     
# ---------------------------------------------------------

# Sur le Mac : récupérer les headers pour le Docker
```bash 
scp -r debian@192.168.6.2:/usr/src/linux-headers-4.19.94-ti-r42 ~/bbb-kernel/
```

# Vérification (Mac)
```bash 
ls ~/bbb-kernel
```
# -> doit contenir include/, scripts/, Makefile, etc.


# ---------------------------------------------------------
#   3. LANCER DOCKER POUR CROSS-COMPILE LE MODULE .KO
# ---------------------------------------------------------

```bash 
docker run --rm -it \
  -v "$(pwd)":/src \
  -v "$(pwd)/kernel_headers/usr/src/linux-headers-4.19.94-ti-r42":/kernel \
  arm32v7/debian:bookworm bash
```


# ---------------------------------------------------------
#   4. INSTALLER GCC / MAKE DANS LE CONTENEUR
# ---------------------------------------------------------
```bash 
apt update
apt install -y build-essential gcc make
```


# ---------------------------------------------------------
#   5. COMPILER LE MODULE NOYAU
# ---------------------------------------------------------
```bash 
cd /src
make
```


# ---------------------------------------------------------
#   6. QUITTER DOCKER
# ---------------------------------------------------------
```bash 
exit
```



# ---------------------------------------------------------
#   7. TRANSFÉRER LE MODULE COMPILÉ SUR LA BBB
# ---------------------------------------------------------

```bash 
scp gpio_pwm_combined.ko debian@192.168.6.2:/home/debian/
```



# ---------------------------------------------------------
#   8. INSÉRER LE MODULE DANS LE NOYAU (TEST)
# ---------------------------------------------------------

```bash 
ssh debian@192.168.6.2
sudo insmod gpio_pwm_combined.ko
```

# Vérifier
```bash 
dmesg | tail -30
```


# ---------------------------------------------------------
#   9. COPIER LES MODULES .KO DANS L’ARBORESCENCE DU NOYAU
# ---------------------------------------------------------

```bash 
sudo mkdir -p /lib/modules/$(uname -r)/extra
sudo cp gpio_blink.ko pwm_kernel.ko /lib/modules/$(uname -r)/extra/
sudo depmod -a
```


# ---------------------------------------------------------
#   10. CRÉER UN FICHIER /etc/modules-load.d POUR CHARGER
#      LES MODULES AUTOMATIQUEMENT AU BOOT
# ---------------------------------------------------------

```bash 
echo "gpio_blink"   | sudo tee /etc/modules-load.d/gpio_pwm.conf
echo "pwm_kernel"   | sudo tee -a /etc/modules-load.d/gpio_pwm.conf
```


# ---------------------------------------------------------
#   11. REDÉMARRER POUR TESTER LE CHARGEMENT AUTOMATIQUE
# ---------------------------------------------------------

```bash 
sudo reboot
```


# ---------------------------------------------------------
#   12. VÉRIFIER APRÈS REBOOT QUE LES MODULES SONT CHARGÉS
# ---------------------------------------------------------

```bash 
lsmod | grep gpio
lsmod | grep pwm

dmesg | grep gpio_blink
dmesg | grep pwm_kernel
```

![alt text](SCR01.PNG)

#  Temps total de boot (kernel + user space)
```bash 
systemd-analyze
```

#  Temps de chaque service
```bash 
systemd-analyze blame
```