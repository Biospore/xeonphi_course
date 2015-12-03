from PyQt4 import QtGui, QtCore
import sys
import random


rcolor = lambda: random.randint(0,255)

class win(QtGui.QWidget):
    def __init__(self, resolution, *args):
        super(win, self).__init__()
        self.resolution = resolution
        self.setGeometry(0, 0, self.resolution[0], self.resolution[1])
        self.datasets = []
        for arg in args[0]:
            try:
                data = []
                fh = open(arg)
                for line in fh.readlines():
                    coord = line.split(' ')
                    coord[0] = coord[0].split('.')[0]
                    coord[1] = coord[1].split('.')[0]
                    data.append(coord)
                self.datasets.append(data)
            except:
                pass
        self.show()


    def paintEvent(self, e):

        qp = QtGui.QPainter()
        qp.begin(self)
        self.drawPoints(qp)
        qp.end()

    def drawPoints(self, qp):
        leng = len(self.datasets)
        colors = []
        for i in range(leng):
            colors.append(QtGui.QColor(rcolor(), rcolor(), rcolor(), 255))
        pen2 = QtGui.QPen(QtCore.Qt.blue, 4, QtCore.Qt.SolidLine)
        pen3 = QtGui.QPen(QtCore.Qt.black, 1, QtCore.Qt.SolidLine)
        try:
            qp.setPen(pen3)
            qp.drawLine(960, 0, 960, 1080)
            qp.drawLine(0, 540, 1920, 540)
            i = 0
            for data in self.datasets:
                ox = 960
                oy = 540
                color = colors[i]
                i+=1
                for x,y in data:
                    pen = QtGui.QPen(color, 2, QtCore.Qt.SolidLine)
                    qp.setPen(pen)
                    x = int(x)/10 + 960
                    y = int(y)/10 + 540
                    qp.drawLine(ox, oy, x, y)
                    #qp.setPen(pen2)
                    #qp.drawPoint(x, y)
                    ox = x
                    oy = y
        except Exception as e:
            print(e)
            pass



def main():
	app = QtGui.QApplication(sys.argv)
	r = app.desktop().screenGeometry()
	wdh = win((r.width(), r.height()), sys.argv[1:])
	sys.exit(app.exec_())


if __name__ == '__main__':
	main()
