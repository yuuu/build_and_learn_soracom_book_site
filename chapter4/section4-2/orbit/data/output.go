package data

//go:generate json-ice --type=Output
type Output struct {
	Message string `json:"message"`
}

func (o *Output) SerializeJSON() ([]byte, error) {
	return MarshalOutputAsJSON(o)
}
