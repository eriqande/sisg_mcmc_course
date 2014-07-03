ACLOCAL_AMFLAGS = -I m4
AUTOMAKE_OPTIONS = subdir-objects

SUBDIRS = eca-shared

bin_PROGRAMS = tpm beta_sim mc3demo



# CFLAGS for each target
tpm_CFLAGS =  $(GL_CFLAGS) $(GLU_CFLAGS) $(GLUT_CFLAGS) -Ieca-shared/include -Wno-deprecated
beta_sim_CFLAGS =  $(GL_CFLAGS) $(GLU_CFLAGS) $(GLUT_CFLAGS) -Ieca-shared/include -Wno-deprecated
mc3demo_CFLAGS =  $(GL_CFLAGS) $(GLU_CFLAGS) $(GLUT_CFLAGS) -Ieca-shared/include -Wno-deprecated

# LDFLAGS for each target
tpm_LDFLAGS = $(GL_LIBS) $(GLU_LIBS) $(GLUT_LIBS) -lm
beta_sim_LDFLAGS = $(GL_LIBS) $(GLU_LIBS) $(GLUT_LIBS) -lm
mc3demo_LDFLAGS = $(GL_LIBS) $(GLU_LIBS) $(GLUT_LIBS) -lm

# which convenience libraries to link to each target
tpm_LDADD = eca-shared/libecashared.a  eca-shared/libgfmcmc.a
beta_sim_LDADD = eca-shared/libecashared.a  eca-shared/libgfmcmc.a
mc3demo_LDADD = eca-shared/libecashared.a  eca-shared/libgfmcmc.a




# sources for each target
tpm_SOURCES = src/tpm.c
beta_sim_SOURCES = src/beta_sim.c 
mc3demo_SOURCES = src/mc3demo.c
