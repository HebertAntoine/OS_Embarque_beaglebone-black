# ---------------------------------------------------------
#   1. RÉCUPÉRER LE NIVEAU DU NOYAU DE LA BBB   
# ---------------------------------------------------------

# Sur la BeagleBone
uname -r
# Exemple : 4.19.94-ti-r42

# Vérifier la présence des headers
ls -l /lib/modules/$(uname -r)/build
# (Sur Debian BBB, il n'existe pas → il faut les installer)

# Télécharger manuellement le .deb (depuis un PC avec internet)
# Fichier utilisé :
# linux-headers-4.19.94-ti-r42_1stretch_armhf.deb

# Copier ce fichier vers la BBB
scp linux-headers-4.19.94-ti-r42_1stretch_armhf.deb debian@192.168.6.2:/home/debian/

# Sur la BeagleBone : installation des headers
sudo dpkg -i linux-headers-4.19.94-ti-r42_1stretch_armhf.deb

# Vérifier que le kernel est maintenant accessible :
ls -l /lib/modules/4.19.94-ti-r42/build
# Doit afficher un lien -> /usr/src/linux-headers-4.19.94-ti-r42

# ---------------------------------------------------------
#   2. COPIER LES HEADERS DE LA BBB VERS LE MAC (CROSS)     
# ---------------------------------------------------------

# Sur le Mac : récupérer les headers pour le Docker
scp -r debian@192.168.6.2:/usr/src/linux-headers-4.19.94-ti-r42 ~/bbb-kernel/

# Vérification (Mac)
ls ~/bbb-kernel
# -> doit contenir include/, scripts/, Makefile, etc.


# ---------------------------------------------------------
#   3. LANCER DOCKER POUR CROSS-COMPILE LE MODULE .KO
# ---------------------------------------------------------

docker run --rm -it \
  -v "$(pwd)":/src \
  -v "$(pwd)/kernel_headers/usr/src/linux-headers-4.19.94-ti-r42":/kernel \
  arm32v7/debian:bookworm bash


# ---------------------------------------------------------
#   4. INSTALLER GCC / MAKE DANS LE CONTENEUR
# ---------------------------------------------------------
apt update
apt install -y build-essential gcc make


# ---------------------------------------------------------
#   5. COMPILER LE MODULE NOYAU
# ---------------------------------------------------------
cd /src
make


# ---------------------------------------------------------
#   6. QUITTER DOCKER
# ---------------------------------------------------------
exit



# ---------------------------------------------------------
#   7. TRANSFÉRER LE MODULE COMPILÉ SUR LA BBB
# ---------------------------------------------------------

scp gpio_pwm_combined.ko debian@192.168.6.2:/home/debian/



# ---------------------------------------------------------
#   8. INSÉRER LE MODULE DANS LE NOYAU (TEST)
# ---------------------------------------------------------

ssh debian@192.168.6.2
sudo insmod gpio_pwm_combined.ko

# Vérifier
dmesg | tail -30


# ---------------------------------------------------------
#   9. COPIER LES MODULES .KO DANS L’ARBORESCENCE DU NOYAU
# ---------------------------------------------------------

sudo mkdir -p /lib/modules/$(uname -r)/extra
sudo cp gpio_blink.ko pwm_kernel.ko /lib/modules/$(uname -r)/extra/
sudo depmod -a


# ---------------------------------------------------------
#   10. CRÉER UN FICHIER /etc/modules-load.d POUR CHARGER
#      LES MODULES AUTOMATIQUEMENT AU BOOT
# ---------------------------------------------------------

echo "gpio_blink"   | sudo tee /etc/modules-load.d/gpio_pwm.conf
echo "pwm_kernel"   | sudo tee -a /etc/modules-load.d/gpio_pwm.conf


# ---------------------------------------------------------
#   11. REDÉMARRER POUR TESTER LE CHARGEMENT AUTOMATIQUE
# ---------------------------------------------------------

sudo reboot


# ---------------------------------------------------------
#   12. VÉRIFIER APRÈS REBOOT QUE LES MODULES SONT CHARGÉS
# ---------------------------------------------------------

lsmod | grep gpio
lsmod | grep pwm

dmesg | grep gpio_blink
dmesg | grep pwm_kernel