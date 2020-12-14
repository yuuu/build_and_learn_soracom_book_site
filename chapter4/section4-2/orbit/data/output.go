package data

//go:generate json-ice --type=Output
type Output struct {
	Status int `json:"status"`
}

func (o *Output) SerializeJSON() ([]byte, error) {
	return MarshalOutputAsJSON(o)
}
