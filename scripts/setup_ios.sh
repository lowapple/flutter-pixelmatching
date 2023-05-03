

src="../src"
ios="../ios/Classes/src"

# remove the source files
rm -rf $ios
# create the source directories
mkdir -p $ios
# copy files
rsync -av "$src/" "$ios/"
