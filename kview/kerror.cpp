
/*
	kview error handler class implementation.
	Sirtaj Singh Kang, March 1997.

	$Id$
*/

#include "kerror.moc"

#include<qapp.h>
#include<qdialog.h>
#include<qlabel.h>
#include<qpushbt.h>

class KViewMsgDlg : public QDialog
{
public:
	KViewMsgDlg(const char *msg=0, QWidget *parent=0, const char *name=0);
private:
	QPushButton	*bt;
	QLabel		*label;
};

KViewMsgDlg::KViewMsgDlg(const char *msg, QWidget *parent, const char *name)
	: QDialog(parent, name)
{

	bt = new QPushButton(this);
	bt->setText("Oops");

	label = new QLabel(this);

	label->move(10,10);
	label->setAutoResize(TRUE);
	label->setText(msg);
	
	resize(10+label->width()+10,
		10+label->height()+10+bt->height()+10);

	bt->move((width()-bt->width())/2, 20+label->height());

	setCaption("kview message");

	connect(bt, SIGNAL(clicked()), this, SLOT(accept()) );

	// move dialog to screen centre

	move( (qApp->desktop()->width()-width())/2,
		(qApp->desktop()->height()-height())/2);

}

const char *KViewError::errormsgs[] = {
        "Unknown Error",

        "kview couldn't find the file.",
	
	"kview was denied read access to the file.",

        "kview didn't recognize the image format.",

	"That's not a valid URL.",

	"kview couldn't recognize the remote file.",

        "kview is already performing an IO operation."
        "Wait for the job to finish, or cancel it.",
	"Couldn't run or connect to KFM.\n"
	"Wont be able to load internet files."
}; 

bool KViewError::message(Type err)
{
	KViewMsgDlg *dialog = new KViewMsgDlg(errormsgs[err]);
	
	dialog->show();

	return TRUE;
}

void KViewError::showMessage(Type err)
{
//	debug("%s:: showMessage called with %d.", className(), err);
	message(err);
}
