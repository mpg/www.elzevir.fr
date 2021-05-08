# Makefile pour la conversion oueb -> html et autres tâches de maintenance

O2H = ./oueb2html
#DEST = sftp://people.math.jussieu.fr/public_html
#DEST = sftp://thue.elzevir.fr/home/mpg/www/elzevir/htdocs/imj

SRC = $(shell find . -name '*.oueb')
HTML = $(patsubst %.oueb, %.html, $(SRC))
SPLIT = $(filter %/head.html, $(HTML))
HTMLADD = $(patsubst %/head.html, %/foot.html, $(SPLIT))
HTACCESS = $(patsubst %/head.html, %/.htaccess, $(SPLIT))
BADHTML = $(patsubst %.html, %.pretidy.html, $(HTML))

all: $(HTML)

%.html: %.oueb oueb2html plan
	$(O2H) $<

clean:
	@echo 'Removing intermediate files'
	@rm -f -- $(BADHTML)

mrproper:
	@echo 'Removing all generated files'
	@rm -f -- $(HTML) $(HTMLADD) $(HTACCESS) $(BADHTML)

#sync:
#	@lftp -e 'mirror -R -v -e -x .git; quit' $(DEST) | cat
#	@# | cat pour éviter les barres de progression et autres

rsync:
	@rsync -avz --delete --exclude=/.git* ./ thue2:/var/www/elzevir/htdocs/imj/

git-setup:
	echo "Order Allow,Deny" > .git/.htaccess
	ln -s ../../post-commit .git/hooks

.PHONY: all clean mrproper sync git-setup
