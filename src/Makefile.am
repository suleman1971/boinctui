bin_PROGRAMS = boinctui

boinctui_SOURCES = \
          ncolorstring.cpp \
          nhline.cpp \
          nvline.cpp \
          nscrollview.cpp \
          nselectlist.cpp \
          nview.cpp \
          ngroup.cpp \
          nstatictext.cpp \
          nform.cpp \
          nprogram.cpp \
          nmenu.cpp \
          nmessagebox.cpp \
          nscrollbar.cpp \
          kclog.cpp \
          cfg.cpp \
          srvdata.cpp \
          mbstring.cpp \
          net.cpp \
          resultparse.cpp \
          resultdom.cpp \
          mainprog.cpp \
          infopanel.cpp \
          mainwin.cpp \
          msgwin.cpp \
          taskwin.cpp \
          tui-main.cpp  \
          cfgform.cpp \
          topmenu.cpp \
          about.cpp \
          helpwin.cpp \
          addprojectform.cpp \
          addmgrform.cpp \
          taskinfowin.cpp \
          statwin.cpp \
          $(NULL)

boinctui_CXXFLAGS = \
         $(BOINCTUI_CXXFLAGS) \
         $(CRYPTO_CXXFLAGS) \
         $(NULL)

boinctui_LDFLAGS = -pthread

boinctui_LDADD = \
         $(BOINCTUI_LIBS) \
         $(CRYPTO_LIBS) \
         $(NULL)
